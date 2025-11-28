// ButtonBase.inl - 模板函数实现
// 此文件由 ButtonBase.h 包含

// ========== 依赖属性定义 ==========

template<typename Derived>
const binding::DependencyProperty &ButtonBase<Derived>::MouseOverBackgroundProperty()
{
    static const auto &prop = binding::DependencyProperty::Register(
        "MouseOverBackground",
        typeid(Brush *),
        typeid(ButtonBase<Derived>),
        binding::PropertyMetadata{
            .defaultValue = static_cast<Brush *>(new SolidColorBrush(Color::FromRGB(16, 110, 190, 255))),
            .propertyChangedCallback = [](binding::DependencyObject &d, const binding::DependencyProperty &, const std::any &, const std::any &)
            {
                auto *button = static_cast<ButtonBase<Derived> *>(&d);
                if (animation::VisualStateManager::GetVisualStateManager(button))
                {
                    button->InitializeVisualStates();
                }
            }});
    return prop;
}

template<typename Derived>
const binding::DependencyProperty &ButtonBase<Derived>::PressedBackgroundProperty()
{
    static const auto &prop = binding::DependencyProperty::Register(
        "PressedBackground",
        typeid(Brush *),
        typeid(ButtonBase<Derived>),
        binding::PropertyMetadata{
            .defaultValue = static_cast<Brush *>(new SolidColorBrush(0, 90, 158, 255)),
            .propertyChangedCallback = [](binding::DependencyObject &d, const binding::DependencyProperty &, const std::any &, const std::any &)
            {
                auto *button = static_cast<ButtonBase<Derived> *>(&d);
                if (animation::VisualStateManager::GetVisualStateManager(button))
                {
                    button->InitializeVisualStates();
                }
            }});
    return prop;
}

template<typename Derived>
const binding::DependencyProperty &ButtonBase<Derived>::PrimaryClickOnlyProperty()
{
    static const auto &prop = binding::DependencyProperty::Register(
        "PrimaryClickOnly",
        typeid(bool),
        typeid(ButtonBase<Derived>),
        binding::PropertyMetadata{
            .defaultValue = std::any(true)
        });
    return prop;
}

// ========== 事件处理 ==========

template<typename Derived>
void ButtonBase<Derived>::OnTemplateApplied()
{
    ContentControl<Derived>::OnTemplateApplied();
    InitializeVisualStates();
}

template<typename Derived>
void ButtonBase<Derived>::OnPropertyChanged(const binding::DependencyProperty &property,
                                           const std::any &oldValue,
                                           const std::any &newValue,
                                           binding::ValueSource oldSource,
                                           binding::ValueSource newSource)
{
    ContentControl<Derived>::OnPropertyChanged(property, oldValue, newValue, oldSource, newSource);

    if (property.Name() == "IsEnabled")
    {
        UpdateVisualState(true);
    }
}

template<typename Derived>
void ButtonBase<Derived>::OnPointerPressed(PointerEventArgs &e)
{
    ContentControl<Derived>::OnPointerPressed(e);

    if (!this->GetIsEnabled())
    {
        return;
    }

    if (GetPrimaryClickOnly() && !e.IsLeftButton())
    {
        return;
    }

    isPressed_ = true;
    pressedButton_ = e.button;
    UpdateVisualState(true);
    e.handled = true;
}

template<typename Derived>
void ButtonBase<Derived>::OnPointerReleased(PointerEventArgs &e)
{
    ContentControl<Derived>::OnPointerReleased(e);

    if (!this->GetIsEnabled())
    {
        isPressed_ = false;
        pressedButton_ = MouseButton::None;
        return;
    }

    bool isPrimaryRelease = e.IsLeftButton();
    if (isPressed_ && pressedButton_ == e.button && (!GetPrimaryClickOnly() || isPrimaryRelease))
    {
        isPressed_ = false;
        pressedButton_ = MouseButton::None;
        UpdateVisualState(true);

        OnClick();
        e.handled = true;
    }
    else
    {
        isPressed_ = false;
        pressedButton_ = MouseButton::None;
    }
}

template<typename Derived>
void ButtonBase<Derived>::OnPointerEntered(PointerEventArgs &e)
{
    ContentControl<Derived>::OnPointerEntered(e);
    UpdateVisualState(true);
}

template<typename Derived>
void ButtonBase<Derived>::OnPointerExited(PointerEventArgs &e)
{
    ContentControl<Derived>::OnPointerExited(e);

    if (isPressed_)
    {
        isPressed_ = false;
        pressedButton_ = MouseButton::None;
    }
    UpdateVisualState(true);
}

template<typename Derived>
void ButtonBase<Derived>::OnClick()
{
    Click();
}

// ========== 视觉状态管理 ==========

template<typename Derived>
void ButtonBase<Derived>::InitializeVisualStates()
{
    if (LoadVisualStatesFromTemplate())
    {
        ResolveVisualStateTargets();
        UpdateVisualState(false);
    }
}

template<typename Derived>
bool ButtonBase<Derived>::LoadVisualStatesFromTemplate()
{
    auto *tmpl = this->GetTemplate();
    
    if (!tmpl || !tmpl->HasVisualStates())
    {
        return false;
    }

    // 检查是否已经有 VisualStateManager，避免重复初始化
    auto* existingManager = animation::VisualStateManager::GetVisualStateManager(this);
    if (existingManager && existingManager->GetStateGroups().size() > 0)
    {
        // 已经初始化过了，直接返回 true
        return true;
    }
    
    auto manager = std::make_shared<animation::VisualStateManager>();
    animation::VisualStateManager::SetVisualStateManager(this, manager);

    // 克隆每个 VisualStateGroup，避免多个控件共享同一个实例
    for (const auto &group : tmpl->GetVisualStateGroups())
    {
        if (group)
        {
            manager->AddStateGroup(group->Clone());
        }
    }

    return true;
}

template<typename Derived>
void ButtonBase<Derived>::UpdateVisualState(bool useTransitions)
{
    std::string targetState;
    if (!this->GetIsEnabled())
    {
        targetState = "Disabled";
    }
    else if (isPressed_)
    {
        targetState = "Pressed";
    }
    else if (this->IsMouseOver())
    {
        targetState = "MouseOver";
    }
    else
    {
        targetState = "Normal";
    }
    
    // 实际调用 GoToState 切换到目标状态
    animation::VisualStateManager::GoToState(this, targetState, useTransitions);
}

template<typename Derived>
void ButtonBase<Derived>::ResolveVisualStateTargets()
{
    auto *manager = animation::VisualStateManager::GetVisualStateManager(this);
    if (!manager)
    {
        return;
    }

    auto *root = this->GetTemplateRoot();
    if (!root)
    {
        return;
    }

    for (auto &group : manager->GetStateGroups())
    {
        if (!group)
            continue;

        for (auto &state : group->GetStates())
        {
            if (!state)
                continue;

            auto storyboard = state->GetStoryboard();
            if (!storyboard)
                continue;

            animation::Storyboard::SetTemplateRoot(storyboard.get(), root);

            for (auto &child : storyboard->GetChildren())
            {
                if (!child)
                    continue;

                std::string targetName = animation::Storyboard::GetTargetName(child.get());
                if (targetName.empty())
                {
                    continue;
                }

                auto *target = ControlTemplate::FindName(targetName, root);
                if (!target)
                {
                    // 只对RootBorder以外的元素发出警告
                    if (targetName != "RootBorder") {
                        std::cerr << "警告：在模板中找不到名为 '" << targetName << "' 的元素\n";
                    }
                    continue;
                }

                std::string propertyPath = animation::Storyboard::GetTargetProperty(child.get());

                if (propertyPath == "Background.Color")
                {
                    auto *border = dynamic_cast<Border *>(target);
                    if (border)
                    {
                        auto *bg = border->GetBackground();
                        auto *brush = dynamic_cast<SolidColorBrush *>(bg);
                        if (brush)
                        {
                            auto *colorAnim = dynamic_cast<animation::ColorAnimation *>(child.get());
                            if (colorAnim)
                            {
                                colorAnim->SetTarget(brush, &SolidColorBrush::ColorProperty());

                                if (colorAnim->HasToBinding())
                                {
                                    auto *bindingProperty = colorAnim->GetToBinding();
                                    auto value = this->GetValue(*bindingProperty);
                                    if (value.has_value())
                                    {
                                        auto *brush = std::any_cast<Brush *>(value);
                                        auto *solidBrush = dynamic_cast<SolidColorBrush *>(brush);
                                        if (solidBrush)
                                        {
                                            colorAnim->SetTo(solidBrush->GetColor());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (propertyPath == "BorderBrush.Color")
                {
                    auto *border = dynamic_cast<Border *>(target);
                    if (border)
                    {
                        auto *brush = border->GetBorderBrush();
                        auto *solidBrush = dynamic_cast<SolidColorBrush *>(brush);
                        if (solidBrush)
                        {
                            auto *colorAnim = dynamic_cast<animation::ColorAnimation *>(child.get());
                            if (colorAnim)
                            {
                                colorAnim->SetTarget(solidBrush, &SolidColorBrush::ColorProperty());

                                if (colorAnim->HasToBinding())
                                {
                                    auto *bindingProperty = colorAnim->GetToBinding();
                                    auto value = this->GetValue(*bindingProperty);
                                    if (value.has_value())
                                    {
                                        auto *brush = std::any_cast<Brush *>(value);
                                        auto *solidBrush = dynamic_cast<SolidColorBrush *>(brush);
                                        if (solidBrush)
                                        {
                                            colorAnim->SetTo(solidBrush->GetColor());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (propertyPath == "Opacity")
                {
                    auto *doubleAnim = dynamic_cast<animation::DoubleAnimation *>(child.get());
                    if (doubleAnim)
                    {
                        doubleAnim->SetTarget(target, &UIElement::OpacityProperty());
                    }
                }
                else if (propertyPath == "Width")
                {
                    auto *doubleAnim = dynamic_cast<animation::DoubleAnimation *>(child.get());
                    if (doubleAnim)
                    {
                        // Border继承自FrameworkElement<Border>，可以直接访问WidthProperty
                        auto *border = dynamic_cast<Border *>(target);
                        if (border)
                        {
                            doubleAnim->SetTarget(border, &Border::WidthProperty());
                        }
                    }
                }
                else if (propertyPath == "Height")
                {
                    auto *doubleAnim = dynamic_cast<animation::DoubleAnimation *>(child.get());
                    if (doubleAnim)
                    {
                        // Border继承自FrameworkElement<Border>，可以直接访问HeightProperty
                        auto *border = dynamic_cast<Border *>(target);
                        if (border)
                        {
                            doubleAnim->SetTarget(border, &Border::HeightProperty());
                        }
                    }
                }
                else if (propertyPath == "Stroke.Color")
                {
                    // 处理Path的Stroke.Color（用于CheckBox的对勾）
                    auto *path = dynamic_cast<Path *>(target);
                    if (path)
                    {
                        auto *brush = path->GetStroke();
                        auto *solidBrush = dynamic_cast<SolidColorBrush *>(brush);
                        if (solidBrush)
                        {
                            auto *colorAnim = dynamic_cast<animation::ColorAnimation *>(child.get());
                            if (colorAnim)
                            {
                                colorAnim->SetTarget(solidBrush, &SolidColorBrush::ColorProperty());
                            }
                        }
                    }
                }
            }
        }
    }
}
