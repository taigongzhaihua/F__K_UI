/**
 * @file main.cpp
 * @brief Binding Syntax Demo - 演示新的链式绑定语法
 *
 * 本示例演示如何使用新的 bind() 辅助函数实现链式绑定语法
 * 例如：textBlock()->Text(bind("PropertyName"))
 */

#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/Button.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Grid.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/TextBox.h"
#include "fk/binding/Binding.h"
#include "fk/binding/ObservableObject.h"
#include "fk/binding/ViewModelMacros.h"

#include <iostream>
#include <memory>
#include <sstream>

// 简单的 ViewModel
class PersonViewModel : public fk::ObservableObject
{
public:
    PersonViewModel()
    {
        SetName("张三");
        SetAge(25);
        SetCity("北京");
    }

    FK_PROPERTY(std::string, Name)
    FK_PROPERTY(int, Age)
    FK_PROPERTY(std::string, City)

public:
    // 增加年龄
    void IncrementAge()
    {
        SetAge(GetAge() + 1);
    }

    // 更改城市
    void ChangeCity()
    {
        static int cityIndex = 0;
        const std::string cities[] = {"北京", "上海", "广州", "深圳", "杭州"};
        cityIndex = (cityIndex + 1) % 5;
        SetCity(cities[cityIndex]);
    }
};

// 注册 ViewModel 的属性 - 使用 FK_VIEWMODEL_AUTO 简化注册
FK_VIEWMODEL_AUTO(PersonViewModel, Name, Age, City)

int main()
{
    using namespace fk;

    try
    {
        // 创建应用程序
        Application app;

        // 创建 ViewModel
        auto viewModel = std::make_shared<PersonViewModel>();

        // 订阅应用程序事件
        app.Startup += [viewModel]
        {
            std::cout << "Application started!" << std::endl;
            std::cout << "初始值 - Name: " << viewModel->GetName()
                      << ", Age: " << viewModel->GetAge()
                      << ", City: " << viewModel->GetCity() << std::endl;
        };

        app.Exit += []
        {
            std::cout << "Application exiting..." << std::endl;
        };

        auto mainWindow = ui::window()
                              ->Title("链式绑定语法演示")
                              ->Width(700)
                              ->Height(500)
                              ->Content(
                                  ui::stackPanel()
                                      ->Orientation(ui::Orientation::Vertical)
                                      ->Spacing(20.0f)
                                      ->Children(
                                          {ui::grid()
                                               ->ColumnDefinitions({ui::ColumnDefinition{ui::GridLength::Auto()},
                                                                    ui::ColumnDefinition{ui::GridLength::Star()}})
                                               ->RowDefinitions({ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()},
                                                                 ui::RowDefinition{ui::GridLength::Auto()}})
                                               ->Children(
                                                   {ui::textBlock()
                                                            ->Text("链式绑定语法演示")
                                                            ->FontSize(28.0f)
                                                            ->Foreground("#FF2E86") |
                                                        ui::cell(0, 0, 1, 2),
                                                    ui::textBlock()
                                                            ->Text("使用新的链式绑定语法 textBlock()->Text(bind(\"Property\"))")
                                                            ->FontSize(14.0f)
                                                            ->Foreground("#FF6666") |
                                                        ui::cell(1, 0, 1, 2),
                                                    ui::textBlock()
                                                            ->Text("姓名:")
                                                            ->FontSize(18.0f)
                                                            ->Foreground("#FF107C") |
                                                        ui::cell(2, 0),
                                                    ui::textBlock()
                                                            ->Text(bind("Name"))
                                                            ->FontSize(24.0f)
                                                            ->Foreground("#FF0078D4") |
                                                        ui::cell(2, 1),
                                                    ui::textBlock()
                                                            ->Text("年龄:")
                                                            ->FontSize(18.0f)
                                                            ->Foreground("#FF107C") |
                                                        ui::cell(4, 0),
                                                    ui::textBlock()
                                                            ->Text(bind("Age"))
                                                            ->FontSize(24.0f)
                                                            ->Foreground("#FF00A3") |
                                                        ui::cell(4, 1),
                                                    ui::textBlock()
                                                            ->Text("城市:")
                                                            ->FontSize(18.0f)
                                                            ->Foreground("#FF107C") |
                                                        ui::cell(5, 0),
                                                    ui::textBlock()
                                                            ->Text(bind("City"))
                                                            ->FontSize(24.0f)
                                                            ->Foreground("#FF8764B8") |
                                                        ui::cell(5, 1),
                                                    ui::textBlock()
                                                            ->Text("↑ 使用新语法: ->Text(bind(\"City\"))")
                                                            ->FontSize(12.0f)
                                                            ->FontFamily("Courier New")
                                                            ->Foreground("#FF9999") |
                                                        ui::cell(6, 0, 1, 2)}),

                                           // ==================== 分隔线 ====================
                                           ui::textBlock()
                                               ->Text("━━━━━━━━━━━━━━━━━━━━")
                                               ->FontSize(16.0f)
                                               ->Foreground("#FFCCCCCC"),

                                           // ==================== TextBox ====================
                                           ui::textBox()
                                               ->Width(300)
                                               ->Height(30)
                                               ->Text(bind("Name")),

                                           // ==================== 按钮区域 ====================
                                           ui::stackPanel()
                                               ->Orientation(ui::Orientation::Horizontal)
                                               ->Spacing(15.0f)
                                               ->Children({ui::button()
                                                               ->Content("更改姓名")
                                                               ->Width(120)
                                                               ->Height(40)
                                                               ->Name("name"),

                                                           ui::button()
                                                               ->Content("增加年龄")
                                                               ->Width(120)
                                                               ->Height(40)
                                                               ->OnClick(
                                                                   [viewModel](auto &)
                                                                   {
                                        viewModel->IncrementAge();
                                        std::cout << "年龄已增加至: " << viewModel->GetAge() << std::endl; }),

                                                           ui::button()
                                                               ->Content("更改城市")
                                                               ->Width(120)
                                                               ->Height(40)
                                                               ->OnClick([viewModel](auto &)
                                                                         {
                                        viewModel->ChangeCity();
                                        std::cout << "城市已更改为: " << viewModel->GetCity() << " (绑定应自动更新UI)" << std::endl; })}),

                                           // ==================== 提示信息 ====================
                                           ui::textBlock()
                                               ->Text("💡 点击按钮修改数据，观察绑定的 TextBlock 自动更新！")
                                               ->FontSize(14.0f)
                                               ->Foreground("#FF6666"),

                                           // ==================== 代码示例 ====================
                                           ui::textBlock()
                                               ->Text("代码示例:")
                                               ->FontSize(16.0f)
                                               ->Foreground("#FF107C"),

                                           ui::textBlock()
                                               ->Text("auto nameText = ui::textBlock()\n    ->Text(bind(\"Name\"))  // 新语法！\n    ->FontSize(24.0f);")
                                               ->FontSize(12.0f)
                                               ->Foreground("#FF3333")}));

        // ✨ 关键！设置 DataContext
        auto baseViewModel = std::static_pointer_cast<INotifyPropertyChanged>(viewModel);
        mainWindow->SetDataContext(baseViewModel);

        auto btn1 = mainWindow->FindName("name")->As<ui::Button<>>();
        btn1->Click += [viewModel](auto &)
        {
            static int nameIndex = 0;
            const std::string names[] = {"张三", "李四", "王五", "赵六", "孙七"};
            nameIndex = (nameIndex + 1) % 5;
            viewModel->SetName(names[nameIndex]);
            std::cout << "姓名已更改为: " << viewModel->GetName() << " (绑定应自动更新UI)" << std::endl;
        };

        // 🎯 测试其他属性的绑定支持
        // FontSize, Foreground 等也都支持绑定了！

        std::cout
            << "✅ 绑定已设置完成！所有依赖属性都支持绑定语法。\n"
            << std::endl;

        // 注册并运行窗口
        app.AddWindow(mainWindow, "MainWindow");

        std::cout << "✨ 新的链式绑定语法已应用: textBlock()->Text(bind(\"Property\"))" << std::endl;
        std::cout << "🎯 绑定机制：SetContent → SetDataContext → Text(bind(...))" << std::endl;
        std::cout << "点击按钮修改ViewModel，观察绑定是否自动更新UI。\n"
                  << std::endl;

        app.Run(mainWindow);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
