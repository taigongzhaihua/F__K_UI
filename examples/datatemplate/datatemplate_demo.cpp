#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/layouts/StackPanel.h"
#include "fk/ui/lists/ListBox.h"
#include "fk/ui/text/TextBlock.h"
#include "fk/ui/controls/Border.h"
#include "fk/ui/styling/DataTemplate.h"
#include "fk/ui/graphics/Brush.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ObservableProperty.h"
#include <iostream>
#include <memory>
#include <vector>

using namespace fk;
using namespace fk::ui;
using namespace fk::binding;

// ========== 数据模型类 ==========

/**
 * @brief 简单用户数据模型（POCO - Plain Old C++ Object）
 * 
 * 这个类演示 DataTemplate 不需要继承 ObservableObject 或使用 ObservableProperty。
 * DataTemplate 可以用于任意数据类型，与 WPF 一样。
 * 
 * 注意：只有在需要数据绑定（UI 自动更新）时才需要 ObservableProperty。
 * DataTemplate 本身只是将数据转换为可视化元素，不涉及绑定。
 */
struct SimpleUserData {
    std::string name;
    int age;
    std::string email;
    
    SimpleUserData(const std::string& name_val, int age_val, const std::string& email_val)
        : name(name_val), age(age_val), email(email_val) {}
};

/**
 * @brief 可绑定用户数据模型
 * 
 * 这个类演示如何定义一个支持数据绑定的数据模型。
 * 使用 ObservableProperty 模板类替代手动 getter/setter 实现。
 * 
 * 只有在需要双向绑定（如 TextBox 输入同步更新数据）时才需要这种方式。
 */
class BindableUserData : public ObservableObject {
public:
    // 使用 ObservableProperty，自动支持属性变更通知和绑定注册
    ObservableProperty<std::string, BindableUserData> name{this, "Name"};
    ObservableProperty<int, BindableUserData> age{this, "Age"};
    ObservableProperty<std::string, BindableUserData> email{this, "Email"};
    
    BindableUserData(const std::string& name_val, int age_val, const std::string& email_val) {
        // 初始化属性值（注意：这会触发属性变更通知）
        name = name_val;
        age = age_val;
        email = email_val;
    }
    
    // 提供传统的 getter 方法以保持向后兼容
    std::string GetName() const { return name.get(); }
    int GetAge() const { return age.get(); }
    std::string GetEmail() const { return email.get(); }
    
    // 提供传统的 setter 方法以保持向后兼容
    void SetName(const std::string& value) { name.set(value); }
    void SetAge(int value) { age.set(value); }
    void SetEmail(const std::string& value) { email.set(value); }
};

// 为了向后兼容，UserData 使用简单结构体
using UserData = SimpleUserData;

// ========== DataTemplate 示例 ==========

/**
 * @brief 创建一个用户数据的DataTemplate
 * 
 * 这个函数演示了如何创建一个DataTemplate来自定义数据对象的可视化表示
 * 类似于WPF中的DataTemplate定义
 * 
 * 注意：这里使用raw pointer创建UI元素是FK_UI框架的标准做法。
 * 父元素会通过TakeOwnership()获取子元素的所有权并管理其生命周期。
 */
std::shared_ptr<DataTemplate> CreateUserDataTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    // 设置工厂函数，定义如何将数据对象转换为可视化元素
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        // 尝试从dataContext中获取UserData对象
        try {
            auto userData = std::any_cast<std::shared_ptr<UserData>>(dataContext);
            
            // 创建视觉树：使用Border包含一个StackPanel
            auto border = new Border();
            border->SetBackground(new SolidColorBrush(240, 240, 255)); // 淡蓝色背景
            border->SetBorderBrush(new SolidColorBrush(100, 100, 200)); // 深蓝色边框
            border->SetBorderThickness(Thickness(2));
            border->SetPadding(Thickness(10));
            border->SetMargin(Thickness(5));
            
            // 创建垂直堆叠面板
            auto panel = new StackPanel();
            panel->SetOrientation(Orientation::Vertical);
            panel->SetSpacing(5);
            
            // 姓名文本块
            auto nameText = new TextBlock();
            nameText->SetText("姓名: " + userData->name);  // 直接访问成员
            nameText->SetFontSize(18);
            nameText->SetForeground(new SolidColorBrush(0, 0, 100)); // 深蓝色
            panel->AddChild(nameText);
            
            // 年龄文本块
            auto ageText = new TextBlock();
            ageText->SetText("年龄: " + std::to_string(userData->age));  // 直接访问成员
            ageText->SetFontSize(14);
            ageText->SetForeground(new SolidColorBrush(50, 50, 50)); // 深灰色
            panel->AddChild(ageText);
            
            // 邮箱文本块
            auto emailText = new TextBlock();
            emailText->SetText("邮箱: " + userData->email);  // 直接访问成员
            emailText->SetFontSize(14);
            emailText->SetForeground(new SolidColorBrush(50, 50, 50)); // 深灰色
            panel->AddChild(emailText);
            
            border->SetChild(panel);
            return border;
            
        } catch (const std::bad_any_cast&) {
            // 如果转换失败，返回一个错误提示
            auto errorText = new TextBlock();
            errorText->SetText("数据类型错误");
            errorText->SetForeground(new SolidColorBrush(255, 0, 0)); // 红色
            return errorText;
        }
    });
    
    return dataTemplate;
}

/**
 * @brief 创建一个简单的文本DataTemplate
 * 
 * 这个示例展示了一个更简单的DataTemplate，只显示文本
 */
std::shared_ptr<DataTemplate> CreateSimpleTextTemplate() {
    auto dataTemplate = std::make_shared<DataTemplate>();
    
    dataTemplate->SetFactory([](const std::any& dataContext) -> UIElement* {
        try {
            // 假设dataContext是一个字符串
            auto text = std::any_cast<std::string>(dataContext);
            
            auto textBlock = new TextBlock();
            textBlock->SetText(text);
            textBlock->SetFontSize(16);
            textBlock->SetForeground(new SolidColorBrush(0, 0, 0));
            textBlock->SetMargin(Thickness(5));
            
            return textBlock;
            
        } catch (const std::bad_any_cast&) {
            auto errorText = new TextBlock();
            errorText->SetText("无法显示数据");
            errorText->SetForeground(new SolidColorBrush(255, 0, 0));
            return errorText;
        }
    });
    
    return dataTemplate;
}

// ========== 主程序 ==========

int main() {
    std::cout << "=== DataTemplate 演示程序 ===" << std::endl;
    std::cout << "这个示例展示了如何在C++中实现类似WPF DataTemplate的效果" << std::endl;
    std::cout << std::endl;
    
    Application app;
    // 注意：mainWindow使用shared_ptr是因为Application::Run需要WindowPtr参数
    // 而子UI元素使用raw pointer是FK_UI框架的标准模式（父元素会管理其生命周期）
    auto mainWindow = std::make_shared<Window>();
    
    // 创建主布局面板
    auto mainPanel = new StackPanel();
    mainPanel->SetOrientation(Orientation::Vertical);
    mainPanel->SetSpacing(10);
    mainPanel->SetMargin(Thickness(20));
    
    // 标题
    auto title = new TextBlock();
    title->SetText("DataTemplate 示例 - 用户列表");
    title->SetFontSize(24);
    title->SetForeground(new SolidColorBrush(0, 0, 100));
    mainPanel->AddChild(title);
    
    // 说明文字
    auto description = new TextBlock();
    description->SetText("以下列表使用DataTemplate自定义了数据的显示方式");
    description->SetFontSize(14);
    description->SetForeground(new SolidColorBrush(100, 100, 100));
    description->SetMargin(Thickness(0, 0, 0, 10));
    mainPanel->AddChild(description);
    
    // 创建ListBox（注意：由于当前实现限制，我们展示概念而非完全可运行的代码）
    // 在完整实现中，ListBox会使用ItemTemplate来显示每个项目
    
    // 创建示例用户数据
    std::vector<std::shared_ptr<UserData>> users = {
        std::make_shared<UserData>("张三", 28, "zhangsan@example.com"),
        std::make_shared<UserData>("李四", 32, "lisi@example.com"),
        std::make_shared<UserData>("王五", 25, "wangwu@example.com"),
        std::make_shared<UserData>("赵六", 30, "zhaoliu@example.com")
    };
    
    // 创建DataTemplate
    auto userTemplate = CreateUserDataTemplate();
    
    std::cout << "创建了 " << users.size() << " 个用户数据" << std::endl;
    std::cout << "创建了用户DataTemplate" << std::endl;
    
    // 手动演示DataTemplate的使用
    // 在实际应用中，ListBox会自动使用ItemTemplate来实例化每个数据项
    for (const auto& user : users) {
        std::cout << "\n正在为用户实例化视觉树: " << user->name << std::endl;  // 直接访问成员
        
        // 使用DataTemplate实例化视觉树
        UIElement* visualElement = userTemplate->Instantiate(user);
        
        if (visualElement) {
            std::cout << "  成功创建视觉元素" << std::endl;
            // 在实际应用中，这个元素会被添加到ListBox的项目面板中
            mainPanel->AddChild(visualElement);
        } else {
            std::cout << "  创建视觉元素失败" << std::endl;
        }
    }
    
    // 分隔线
    auto separator = new Border();
    separator->SetHeight(2);
    separator->SetBackground(new SolidColorBrush(200, 200, 200));
    separator->SetMargin(Thickness(0, 20, 0, 20));
    mainPanel->AddChild(separator);
    
    // 第二个示例：简单文本模板
    auto simpleTitle = new TextBlock();
    simpleTitle->SetText("简单文本DataTemplate示例");
    simpleTitle->SetFontSize(20);
    simpleTitle->SetForeground(new SolidColorBrush(0, 100, 0));
    mainPanel->AddChild(simpleTitle);
    
    auto simpleTemplate = CreateSimpleTextTemplate();
    std::vector<std::string> messages = {
        "这是第一条消息",
        "这是第二条消息",
        "这是第三条消息"
    };
    
    for (const auto& msg : messages) {
        UIElement* element = simpleTemplate->Instantiate(msg);
        if (element) {
            mainPanel->AddChild(element);
        }
    }
    
    // 底部说明
    auto footer = new TextBlock();
    footer->SetText("提示：DataTemplate允许你自定义任何数据对象的可视化表示");
    footer->SetFontSize(12);
    footer->SetForeground(new SolidColorBrush(150, 150, 150));
    footer->SetMargin(Thickness(0, 20, 0, 0));
    mainPanel->AddChild(footer);
    
    // 配置窗口
    mainWindow->Title("DataTemplate 演示")
        ->Width(600)
        ->Height(800)
        ->Background(new SolidColorBrush(255, 255, 255))
        ->Content(mainPanel);
    
    std::cout << "\n启动应用程序..." << std::endl;
    std::cout << "DataTemplate成功创建了自定义的数据可视化!" << std::endl;
    
    app.Run(mainWindow);
    return 0;
}
