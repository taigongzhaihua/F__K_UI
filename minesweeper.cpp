#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>

using namespace std;

// 游戏常量
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int GRID_SIZE = 10;
const int CELL_SIZE = 40;
const int MINE_COUNT = 15;

// 游戏状态
enum class GameState {
    PLAYING,
    WIN,
    LOSE
};

// 格子状态
enum class CellState {
    HIDDEN,
    REVEALED,
    FLAGGED,
    MINE
};

// 字符结构体
struct Character {
    unsigned int TextureID; // 字符纹理ID
    glm::ivec2   Size;      // 字符大小
    glm::ivec2   Bearing;   // 字符基线偏移
    unsigned int Advance;   // 水平偏移量
};

// 全局字符映射
std::map<GLchar, Character> Characters;

// 文本渲染着色器 - 使用 OpenGL 2.1 兼容语法
const char* textVertexShaderSource = R"(
#version 120
attribute vec4 vertex; // <vec2 pos, vec2 tex>
varying vec2 TexCoords;

uniform mat4 projection;

void main() {
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
}
)";

const char* textFragmentShaderSource = R"(
#version 120
varying vec2 TexCoords;

uniform sampler2D text;
uniform vec3 textColor;

void main() {
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture2D(text, TexCoords).r);
    gl_FragColor = vec4(textColor, 1.0) * sampled;
}
)";

class TextRenderer {
private:
    unsigned int VBO;
    unsigned int shaderProgram;
    
public:
    TextRenderer() {
        cout << "创建 TextRenderer..." << endl;
        
        // 编译着色器
        unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, textVertexShaderSource);
        unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, textFragmentShaderSource);
        
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        
        // 检查链接错误
        int success;
        char infoLog[512];
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            cout << "着色器程序链接失败:\n" << infoLog << endl;
        } else {
            cout << "着色器程序链接成功" << endl;
        }
        
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        
        // 配置VBO用于纹理四边形（OpenGL 2.1 兼容，不使用VAO）
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        cout << "TextRenderer 创建完成" << endl;
    }
    
    void RenderText(string text, float x, float y, float scale, glm::vec3 color) {
        // 保存当前OpenGL状态
        glPushAttrib(GL_ALL_ATTRIB_BITS);
        glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);
        
        // 激活对应渲染状态
        glUseProgram(shaderProgram);
        glUniform3f(glGetUniformLocation(shaderProgram, "textColor"), color.x, color.y, color.z);
        glActiveTexture(GL_TEXTURE0);
        
        // 绑定VBO并设置顶点属性
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        GLint vertexLoc = glGetAttribLocation(shaderProgram, "vertex");
        if (vertexLoc >= 0) {
            glEnableVertexAttribArray(vertexLoc);
            glVertexAttribPointer(vertexLoc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        }
        
        // 遍历文本中所有的字符
        string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Characters[*c];
            
            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
            
            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            
            // 对每个字符更新VBO
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },            
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }           
            };
            
            // 在四边形上绘制字符纹理
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            // 更新VBO内存的内容
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            // 绘制四边形
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // 更新位置到下一个字符的原点
            x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取像素值(2^6 = 64)
        }
        
        // 清理和恢复OpenGL状态
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glUseProgram(0);
        
        // 恢复之前保存的OpenGL状态
        glPopClientAttrib();
        glPopAttrib();
    }
    
    void SetProjection(glm::mat4 projection) {
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUseProgram(0);  // 恢复到固定管线
    }
    
private:
    unsigned int compileShader(unsigned int type, const char* source) {
        unsigned int shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);
        
        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            cout << "文本着色器编译失败: " << infoLog << endl;
        }
        
        return shader;
    }
};

class Minesweeper {
private:
    vector<vector<CellState>> board;
    vector<vector<bool>> mines;
    vector<vector<int>> adjacentMines;
    GameState gameState;
    int flagsPlaced;
    bool firstClick;
    chrono::time_point<chrono::steady_clock> startTime;
    unique_ptr<TextRenderer> textRenderer;
    
public:
    Minesweeper() : board(GRID_SIZE, vector<CellState>(GRID_SIZE, CellState::HIDDEN)),
                   mines(GRID_SIZE, vector<bool>(GRID_SIZE, false)),
                   adjacentMines(GRID_SIZE, vector<int>(GRID_SIZE, 0)),
                   gameState(GameState::PLAYING),
                   flagsPlaced(0),
                   firstClick(true) {}
    
    void initialize(int firstX, int firstY) {
        board = vector<vector<CellState>>(GRID_SIZE, vector<CellState>(GRID_SIZE, CellState::HIDDEN));
        mines = vector<vector<bool>>(GRID_SIZE, vector<bool>(GRID_SIZE, false));
        adjacentMines = vector<vector<int>>(GRID_SIZE, vector<int>(GRID_SIZE, 0));
        gameState = GameState::PLAYING;
        flagsPlaced = 0;
        firstClick = false;
        startTime = chrono::steady_clock::now();
        
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(0, GRID_SIZE - 1);
        
        int minesPlaced = 0;
        while (minesPlaced < MINE_COUNT) {
            int x = dis(gen);
            int y = dis(gen);
            
            if (!mines[x][y] && 
                (x != firstX || y != firstY) && 
                abs(x - firstX) > 1 && abs(y - firstY) > 1) {
                mines[x][y] = true;
                minesPlaced++;
            }
        }
        
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                adjacentMines[x][y] = countAdjacentMines(x, y);
            }
        }
        
        cout << "游戏初始化完成！地雷数量: " << MINE_COUNT << endl;
    }
    
    int countAdjacentMines(int x, int y) {
        int count = 0;
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE && mines[nx][ny]) {
                    count++;
                }
            }
        }
        return count;
    }
    
    void reveal(int x, int y) {
        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || 
            board[x][y] == CellState::REVEALED || board[x][y] == CellState::FLAGGED) {
            return;
        }
        
        if (firstClick) {
            cout << "第一次点击，初始化游戏..." << endl;
            initialize(x, y);
        }
        
        if (mines[x][y]) {
            cout << "踩到地雷！游戏结束" << endl;
            board[x][y] = CellState::REVEALED;
            gameState = GameState::LOSE;
            revealAllMines();
            return;
        }
        
        board[x][y] = CellState::REVEALED;
        
        if (adjacentMines[x][y] == 0) {
            for (int dx = -1; dx <= 1; dx++) {
                for (int dy = -1; dy <= 1; dy++) {
                    if (dx != 0 || dy != 0) {
                        reveal(x + dx, y + dy);
                    }
                }
            }
        }
        
        checkWin();
    }
    
    void revealAllMines() {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (mines[x][y]) {
                    board[x][y] = CellState::REVEALED;
                }
            }
        }
    }
    
    void toggleFlag(int x, int y) {
        if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE || 
            board[x][y] == CellState::REVEALED) {
            return;
        }
        
        if (board[x][y] == CellState::FLAGGED) {
            board[x][y] = CellState::HIDDEN;
            flagsPlaced--;
        } else {
            board[x][y] = CellState::FLAGGED;
            flagsPlaced++;
        }
        
        cout << "标记/取消标记 (" << x << ", " << y << "), 剩余地雷: " << getRemainingMines() << endl;
        
        checkWin();
    }
    
    void checkWin() {
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                if (!mines[x][y] && board[x][y] != CellState::REVEALED) {
                    return;
                }
            }
        }
        gameState = GameState::WIN;
        cout << "恭喜！你赢了！" << endl;
    }
    
    GameState getGameState() const {
        return gameState;
    }
    
    int getElapsedTime() const {
        if (firstClick) return 0;
        auto now = chrono::steady_clock::now();
        return chrono::duration_cast<chrono::seconds>(now - startTime).count();
    }
    
    int getRemainingMines() const {
        return MINE_COUNT - flagsPlaced;
    }
    
    void initTextRenderer() {
        // 创建 TextRenderer 实例
        textRenderer = make_unique<TextRenderer>();
        
        // 设置文本渲染的投影矩阵
        glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(WINDOW_WIDTH), 0.0f, static_cast<float>(WINDOW_HEIGHT));
        textRenderer->SetProjection(projection);
    }
    
    void draw() {
        // 使用传统OpenGL立即模式（兼容模式）
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        
        // 绘制背景
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // 绘制所有格子
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                drawCell(x, y);
            }
        }
        
        // 绘制游戏状态信息（使用FreeType渲染文字）
        drawGameStatus();
    }
    
    void handleClick(double x, double y, int button) {
        int gridX = (x - 50) / CELL_SIZE;
        int gridY = (y - 50) / CELL_SIZE;
        
        if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 && gridY < GRID_SIZE) {
            if (button == GLFW_MOUSE_BUTTON_LEFT && gameState == GameState::PLAYING) {
                cout << "左键点击 (" << gridX << ", " << gridY << ")" << endl;
                reveal(gridX, gridY);
            } else if (button == GLFW_MOUSE_BUTTON_RIGHT && gameState == GameState::PLAYING) {
                cout << "右键点击 (" << gridX << ", " << gridY << ")" << endl;
                toggleFlag(gridX, gridY);
            }
        }
    }
    
    void reset() {
        board = vector<vector<CellState>>(GRID_SIZE, vector<CellState>(GRID_SIZE, CellState::HIDDEN));
        mines = vector<vector<bool>>(GRID_SIZE, vector<bool>(GRID_SIZE, false));
        adjacentMines = vector<vector<int>>(GRID_SIZE, vector<int>(GRID_SIZE, 0));
        gameState = GameState::PLAYING;
        flagsPlaced = 0;
        firstClick = true;
        cout << "游戏已重置" << endl;
    }
    
private:
    void drawCell(int x, int y) {
        float screenX = x * CELL_SIZE + 50.0f;
        float screenY = y * CELL_SIZE + 50.0f;
        
        // 绘制格子背景
        if (board[x][y] == CellState::REVEALED) {
            glColor3f(0.8f, 0.8f, 0.8f); // 已翻开：浅灰色
        } else {
            glColor3f(0.6f, 0.6f, 0.6f); // 未翻开：深灰色
        }
        
        glBegin(GL_QUADS);
        glVertex2f(screenX, screenY);
        glVertex2f(screenX + CELL_SIZE, screenY);
        glVertex2f(screenX + CELL_SIZE, screenY + CELL_SIZE);
        glVertex2f(screenX, screenY + CELL_SIZE);
        glEnd();
        
        // 绘制格子边框
        glColor3f(0.3f, 0.3f, 0.3f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(screenX, screenY);
        glVertex2f(screenX + CELL_SIZE, screenY);
        glVertex2f(screenX + CELL_SIZE, screenY + CELL_SIZE);
        glVertex2f(screenX, screenY + CELL_SIZE);
        glEnd();
        
        // 绘制格子内容
        if (board[x][y] == CellState::REVEALED) {
            if (mines[x][y]) {
                drawMine(screenX, screenY);
            } else if (adjacentMines[x][y] > 0) {
                drawNumber(screenX, screenY, adjacentMines[x][y]);
            }
        } else if (board[x][y] == CellState::FLAGGED) {
            drawFlag(screenX, screenY);
        } else {
            // 未翻开格子的3D效果
            glColor3f(0.9f, 0.9f, 0.9f);
            glBegin(GL_LINES);
            glVertex2f(screenX, screenY);
            glVertex2f(screenX + CELL_SIZE, screenY);
            glVertex2f(screenX, screenY);
            glVertex2f(screenX, screenY + CELL_SIZE);
            glEnd();
            
            glColor3f(0.4f, 0.4f, 0.4f);
            glBegin(GL_LINES);
            glVertex2f(screenX + CELL_SIZE, screenY);
            glVertex2f(screenX + CELL_SIZE, screenY + CELL_SIZE);
            glVertex2f(screenX, screenY + CELL_SIZE);
            glVertex2f(screenX + CELL_SIZE, screenY + CELL_SIZE);
            glEnd();
        }
    }
    
    void drawMine(float x, float y) {
        glColor3f(0.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLE_FAN);
        float centerX = x + CELL_SIZE / 2.0f;
        float centerY = y + CELL_SIZE / 2.0f;
        float radius = CELL_SIZE / 3.0f;
        for (int i = 0; i <= 20; i++) {
            float angle = 2.0f * 3.14159f * i / 20.0f;
            glVertex2f(centerX + radius * cos(angle), centerY + radius * sin(angle));
        }
        glEnd();
        
        // 地雷的十字架
        glColor3f(0.0f, 0.0f, 0.0f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(centerX - radius, centerY);
        glVertex2f(centerX + radius, centerY);
        glVertex2f(centerX, centerY - radius);
        glVertex2f(centerX, centerY + radius);
        glEnd();
    }
    
    void drawNumber(float x, float y, int number) {
        // 在格子中心绘制数字
        string numStr = to_string(number);
        float scale = 0.5f;
        
        // 根据数字设置颜色
        glm::vec3 color;
        switch(number) {
            case 1: color = glm::vec3(0.0f, 0.0f, 1.0f); break; // 蓝色
            case 2: color = glm::vec3(0.0f, 0.5f, 0.0f); break; // 绿色
            case 3: color = glm::vec3(1.0f, 0.0f, 0.0f); break; // 红色
            case 4: color = glm::vec3(0.5f, 0.0f, 0.5f); break; // 紫色
            case 5: color = glm::vec3(0.8f, 0.5f, 0.0f); break; // 橙色
            case 6: color = glm::vec3(0.0f, 0.8f, 0.8f); break; // 青色
            case 7: color = glm::vec3(0.0f, 0.0f, 0.0f); break; // 黑色
            case 8: color = glm::vec3(0.5f, 0.5f, 0.5f); break; // 灰色
            default: color = glm::vec3(0.0f, 0.0f, 0.0f); break;
        }
        
        // 计算文本位置（居中）
        float textX = x + (CELL_SIZE - getTextWidth(numStr, scale)) / 2.0f;
        float textY = WINDOW_HEIGHT - (y + (CELL_SIZE - 20 * scale) / 2.0f + 20 * scale);
        
        textRenderer->RenderText(numStr, textX, textY, scale, color);
    }
    
    void drawFlag(float x, float y) {
        // 旗杆
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineWidth(3.0f);
        glBegin(GL_LINES);
        glVertex2f(x + CELL_SIZE / 2, y + 5);
        glVertex2f(x + CELL_SIZE / 2, y + CELL_SIZE - 5);
        glEnd();
        
        // 旗子
        glColor3f(1.0f, 0.0f, 0.0f);
        glBegin(GL_TRIANGLES);
        glVertex2f(x + CELL_SIZE / 2, y + 10);
        glVertex2f(x + CELL_SIZE / 2 + 10, y + 15);
        glVertex2f(x + CELL_SIZE / 2, y + 20);
        glEnd();
    }
    
    void drawGameStatus() {
        // 检查 textRenderer 是否已初始化
        if (!textRenderer) {
            return;
        }
        
        // 绘制状态栏背景
        glColor3f(0.1f, 0.1f, 0.1f);
        glBegin(GL_QUADS);
        glVertex2f(0, 0);
        glVertex2f(WINDOW_WIDTH, 0);
        glVertex2f(WINDOW_WIDTH, 40);
        glVertex2f(0, 40);
        glEnd();
        
        // 使用FreeType渲染游戏状态文字
        string statusText;
        glm::vec3 statusColor;
        
        if (gameState == GameState::WIN) {
            statusText = "胜利! 时间: " + to_string(getElapsedTime()) + "秒";
            statusColor = glm::vec3(0.0f, 1.0f, 0.0f);
        } else if (gameState == GameState::LOSE) {
            statusText = "失败! 时间: " + to_string(getElapsedTime()) + "秒";
            statusColor = glm::vec3(1.0f, 0.0f, 0.0f);
        } else {
            statusText = "游戏中... 时间: " + to_string(getElapsedTime()) + "秒, 剩余地雷: " + to_string(getRemainingMines());
            statusColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        
        textRenderer->RenderText(statusText, 10.0f, WINDOW_HEIGHT - 25.0f, 0.4f, statusColor);
        
        // 绘制操作说明
        string instructions = "左键:翻开 右键:标记 R:重置 ESC:退出";
        textRenderer->RenderText(instructions, 10.0f, WINDOW_HEIGHT - 10.0f, 0.3f, glm::vec3(1.0f, 1.0f, 1.0f));
    }
    
    float getTextWidth(const string& text, float scale) {
        float width = 0.0f;
        for (char c : text) {
            Character ch = Characters[c];
            width += (ch.Advance >> 6) * scale;
        }
        return width;
    }
};

// 全局变量
Minesweeper game;
GLFWwindow* window;

// 初始化FreeType
bool initFreeType() {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        cout << "错误: 无法初始化FreeType库" << endl;
        return false;
    }
    
    FT_Face face;
    // 尝试多个字体路径
    const char* fontPaths[] = {
        "C:/Windows/Fonts/arial.ttf",
        "C:/Windows/Fonts/simhei.ttf",  // 黑体（支持中文）
        "C:/Windows/Fonts/simsun.ttc",  // 宋体
        "arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"  // Linux路径
    };
    
    bool fontLoaded = false;
    for (const char* fontPath : fontPaths) {
        if (FT_New_Face(ft, fontPath, 0, &face) == 0) {
            cout << "成功加载字体: " << fontPath << endl;
            fontLoaded = true;
            break;
        }
    }
    
    if (!fontLoaded) {
        cout << "错误: 无法加载任何字体文件" << endl;
        cout << "尝试的路径:" << endl;
        for (const char* fontPath : fontPaths) {
            cout << "  - " << fontPath << endl;
        }
        return false;
    }
    
    // 设置字体大小
    FT_Set_Pixel_Sizes(face, 0, 48);
    
    // 禁用字节对齐限制
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // 加载前128个ASCII字符
    for (unsigned char c = 0; c < 128; c++) {
        // 加载字符字形
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            cout << "错误: 无法加载字符 '" << c << "'" << endl;
            continue;
        }
        
        // 生成纹理
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        
        // 设置纹理选项
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // 存储字符供以后使用
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    return true;
}

// 窗口大小已固定，不需要此回调
// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//     glViewport(0, 0, width, height);
// }

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        game.handleClick(xpos, ypos, button);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        game.reset();
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    cout << "正在初始化扫雷游戏..." << endl;
    cout.flush();
    
    // 初始化GLFW
    cout << "初始化 GLFW..." << endl;
    cout.flush();
    if (!glfwInit()) {
        cerr << "Failed to initialize GLFW" << endl;
        cerr.flush();
        system("pause");
        return -1;
    }
    cout << "GLFW 初始化成功" << endl;
    cout.flush();
    
    // 配置GLFW - 使用兼容模式而不是核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    
    // 设置窗口不可调整大小
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    // 创建窗口
    cout << "创建窗口..." << endl;
    cout.flush();
    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "扫雷游戏 - 带FreeType字体渲染", NULL, NULL);
    if (!window) {
        cerr << "Failed to create GLFW window" << endl;
        cerr.flush();
        glfwTerminate();
        system("pause");
        return -1;
    }
    cout << "窗口创建成功" << endl;
    cout.flush();
    
    glfwMakeContextCurrent(window);
    
    // 初始化GLAD
    cout << "初始化 GLAD..." << endl;
    cout.flush();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cerr << "Failed to initialize GLAD" << endl;
        cerr.flush();
        system("pause");
        return -1;
    }
    cout << "GLAD 初始化成功" << endl;
    cout.flush();
    
    // 初始化FreeType
    cout << "初始化 FreeType..." << endl;
    cout.flush();
    if (!initFreeType()) {
        cerr << "Failed to initialize FreeType" << endl;
        cerr.flush();
        system("pause");
        return -1;
    }
    cout << "FreeType 初始化成功" << endl;
    cout.flush();
    
    // 设置回调函数（窗口大小固定，不需要 framebuffer_size_callback）
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // 设置OpenGL状态
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // 初始化文本渲染器
    game.initTextRenderer();
    
    cout << "扫雷游戏已启动！" << endl;
    cout << "操作说明:" << endl;
    cout << "- 左键点击: 翻开格子" << endl;
    cout << "- 右键点击: 标记/取消标记地雷" << endl;
    cout << "- 按R键: 重新开始游戏" << endl;
    cout << "- 按ESC键: 退出游戏" << endl;
    
    // 主循环
    while (!glfwWindowShouldClose(window)) {
        // 处理事件
        glfwPollEvents();
        
        // 渲染
        game.draw();
        
        // 交换缓冲
        glfwSwapBuffers(window);
    }
    
    glfwTerminate();
    return 0;
}