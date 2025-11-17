#include "fk/ui/InputManager.h"
#include "fk/ui/StackPanel.h"
#include "fk/ui/Button.h"
#include "fk/ui/TextBlock.h"
#include "fk/ui/Brush.h"
#include <iostream>

int main() {
    auto* root = new fk::ui::StackPanel();
    root->Width(400);
    root->Height(300);

    auto* button = (new fk::ui::Button())
        ->Name("testButton")
        ->Width(200)
        ->Height(80)
        ->Margin(fk::Thickness(20))
        ->Content((new fk::ui::TextBlock())
                      ->Text("Click Me")
                      ->FontSize(20));

    root->Children({button});

    fk::ui::Size available(400.0f, 300.0f);
    root->Measure(available);
    root->Arrange(fk::ui::Rect(0, 0, available.width, available.height));

    fk::ui::InputManager input;
    input.SetRoot(root);

    bool clicked = false;
    button->Click += [&]() {
        clicked = true;
        std::cout << "Button clicked!" << std::endl;
    };

    fk::ui::Rect layout = button->GetLayoutRect();
    fk::ui::Point inside(layout.x + layout.width * 0.5f, layout.y + layout.height * 0.5f);

    fk::ui::PlatformPointerEvent down;
    down.type = fk::ui::PlatformPointerEvent::Type::Down;
    down.position = inside;
    down.button = 0;
    input.ProcessPointerEvent(down);

    fk::ui::PlatformPointerEvent up = down;
    up.type = fk::ui::PlatformPointerEvent::Type::Up;
    input.ProcessPointerEvent(up);

    delete root;

    if (!clicked) {
        std::cerr << "Click event not triggered" << std::endl;
        return 1;
    }

    return 0;
}
