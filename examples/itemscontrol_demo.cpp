#include "fk/app/Application.h"
#include "fk/ui/Window.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/ItemsControl.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Button.h"
#include <string>

using namespace fk;
using namespace fk::ui;

int main() {
    Application app;
    
    // 创建主窗口
    auto* mainWindow = app.CreateWindow();
    mainWindow->Title("ItemsControl Demo")
              ->Width(600.0f)
              ->Height(400.0f);
    
    // 创建主面板
    auto* panel = new StackPanel();
    panel->Orientation(Orientation::Vertical)
         ->Spacing(10.0f)
         ->Margin(Thickness(20));
    
    // 标题
    auto* title = new TextBlock();
    title->Text("ItemsControl with ObservableCollection")
         ->FontSize(24.0f)
         ->Margin(Thickness(0, 0, 0, 20));
    panel->AddChild(title);
    
    // 创建一个简单的 ItemsControl 派生类用于演示
    class SimpleItemsControl : public ItemsControl<SimpleItemsControl> {};
    
    auto* itemsControl = new SimpleItemsControl();
    
    // 获取 Items 集合并添加项
    auto& items = itemsControl->GetItems();
    
    // 添加一些文本项
    items.Add(std::string("Item 1: First item"));
    items.Add(std::string("Item 2: Second item"));
    items.Add(std::string("Item 3: Third item"));
    
    panel->AddChild(itemsControl);
    
    // 添加按钮用于动态添加项
    auto* addButton = new Button();
    addButton->Content(std::string("Add Item"))
             ->Width(150.0f)
             ->Margin(Thickness(0, 20, 0, 0));
    
    static int itemCount = 4;
    addButton->Click += [&items](ui::detail::ButtonBase& btn) {
        items.Add(std::string("Item " + std::to_string(itemCount++) + ": Dynamically added"));
    };
    
    panel->AddChild(addButton);
    
    // 显示项数量的文本
    auto* countText = new TextBlock();
    countText->Text("Items count: " + std::to_string(items.Count()))
             ->Margin(Thickness(0, 10, 0, 0));
    
    // 监听集合变更事件
    items.CollectionChanged() += [countText, &items](const CollectionChangedEventArgs& args) {
        countText->SetText("Items count: " + std::to_string(items.Count()) + 
                          " (Action: " + std::to_string(static_cast<int>(args.action)) + ")");
    };
    
    panel->AddChild(countText);
    
    // 设置窗口内容
    mainWindow->SetContent(panel);
    
    // 运行应用程序
    app.Run(app.GetMainWindow());
    return 0;
}
