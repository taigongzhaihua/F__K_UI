/**
 * @file ScrollViewer.cpp
 * @brief ScrollViewer 滚动视图控件实现
 */

#include "fk/ui/ScrollViewer.h"
#include "fk/ui/Grid.h"
#include "fk/ui/Border.h"
#include "fk/ui/Brush.h"
#include "fk/ui/ControlTemplate.h"
#include "fk/binding/DependencyProperty.h"
#include "fk/binding/TemplateBinding.h"
#include <algorithm>
#include <cmath>

namespace fk::ui {

// ========== 模板显式实例化 ==========
template class ContentControl<ScrollViewer>;
template class Control<ScrollViewer>;
template class FrameworkElement<ScrollViewer>;

// ========== 默认模板 ==========

static ControlTemplate* CreateScrollViewerTemplate() {
    auto* tmpl = new ControlTemplate();
    tmpl->SetTargetType(typeid(ScrollViewer))
        ->SetFactory([]() -> UIElement* {
            // 2x2 Grid 布局
            // ┌─────────────────┬───┐
            // │ ScrollContent   │ V │
            // │   Presenter     │ S │
            // │                 │ B │
            // ├─────────────────┼───┤
            // │      HSB        │   │
            // └─────────────────┴───┘
            
            auto* grid = new Grid();
            grid->Name("ScrollViewerRoot");
            
            // 定义行和列
            grid->RowDefinitions({
                RowDefinition::Star(1),     // 内容区域
                RowDefinition::Auto()       // 水平滚动条
            });
            grid->ColumnDefinitions({
                ColumnDefinition::Star(1),  // 内容区域
                ColumnDefinition::Auto()    // 垂直滚动条
            });
            
            // ScrollContentPresenter
            auto* presenter = new ScrollContentPresenter_t();
            presenter->Name("PART_ScrollContentPresenter");
            presenter->SetCanVerticallyScroll(true);
            presenter->SetCanHorizontallyScroll(false);
            Grid::SetRow(presenter, 0);
            Grid::SetColumn(presenter, 0);
            grid->AddChild(presenter);
            
            // 垂直滚动条
            auto* vScrollBar = new ScrollBar();
            vScrollBar->Name("PART_VerticalScrollBar");
            vScrollBar->SetOrientation(Orientation::Vertical);
            vScrollBar->Width(16);
            Grid::SetRow(vScrollBar, 0);
            Grid::SetColumn(vScrollBar, 1);
            grid->AddChild(vScrollBar);
            
            // 水平滚动条
            auto* hScrollBar = new ScrollBar();
            hScrollBar->Name("PART_HorizontalScrollBar");
            hScrollBar->SetOrientation(Orientation::Horizontal);
            hScrollBar->Height(16);
            hScrollBar->SetVisibility(Visibility::Collapsed);  // 默认隐藏
            Grid::SetRow(hScrollBar, 1);
            Grid::SetColumn(hScrollBar, 0);
            grid->AddChild(hScrollBar);
            
            return grid;
        });
    
    return tmpl;
}

// ========== 依赖属性定义 ==========

const binding::DependencyProperty& ScrollViewer::HorizontalScrollBarVisibilityProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "HorizontalScrollBarVisibility",
        typeid(ScrollBarVisibility),
        typeid(ScrollViewer),
        binding::PropertyMetadata{
            std::any(ScrollBarVisibility::Disabled),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any&) {
                auto& sv = static_cast<ScrollViewer&>(obj);
                sv.UpdateScrollBarVisibility();
                sv.SyncScrollContentPresenter();
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollViewer::VerticalScrollBarVisibilityProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "VerticalScrollBarVisibility",
        typeid(ScrollBarVisibility),
        typeid(ScrollViewer),
        binding::PropertyMetadata{
            std::any(ScrollBarVisibility::Visible),
            [](DependencyObject& obj, const DependencyProperty&, const std::any&, const std::any&) {
                auto& sv = static_cast<ScrollViewer&>(obj);
                sv.UpdateScrollBarVisibility();
                sv.SyncScrollContentPresenter();
            }
        }
    );
    return prop;
}

const binding::DependencyProperty& ScrollViewer::HorizontalOffsetProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "HorizontalOffset",
        typeid(double),
        typeid(ScrollViewer),
        binding::PropertyMetadata{std::any(0.0)}
    );
    return prop;
}

const binding::DependencyProperty& ScrollViewer::VerticalOffsetProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "VerticalOffset",
        typeid(double),
        typeid(ScrollViewer),
        binding::PropertyMetadata{std::any(0.0)}
    );
    return prop;
}

const binding::DependencyProperty& ScrollViewer::CanContentScrollProperty() {
    static auto& prop = binding::DependencyProperty::Register(
        "CanContentScroll",
        typeid(bool),
        typeid(ScrollViewer),
        binding::PropertyMetadata{std::any(false)}
    );
    return prop;
}

// ========== 构造/析构 ==========

ScrollViewer::ScrollViewer() {
    // 确保依赖属性已注册
    HorizontalScrollBarVisibilityProperty();
    VerticalScrollBarVisibilityProperty();
    HorizontalOffsetProperty();
    VerticalOffsetProperty();
    CanContentScrollProperty();
    
    // 设置默认模板
    SetTemplate(CreateScrollViewerTemplate());
}

ScrollViewer::~ScrollViewer() = default;

// ========== 属性访问器 ==========

ScrollBarVisibility ScrollViewer::GetHorizontalScrollBarVisibility() const {
    return DependencyObject::GetValue<ScrollBarVisibility>(HorizontalScrollBarVisibilityProperty());
}

ScrollViewer* ScrollViewer::SetHorizontalScrollBarVisibility(ScrollBarVisibility value) {
    DependencyObject::SetValue(HorizontalScrollBarVisibilityProperty(), value);
    return this;
}

ScrollBarVisibility ScrollViewer::GetVerticalScrollBarVisibility() const {
    return DependencyObject::GetValue<ScrollBarVisibility>(VerticalScrollBarVisibilityProperty());
}

ScrollViewer* ScrollViewer::SetVerticalScrollBarVisibility(ScrollBarVisibility value) {
    DependencyObject::SetValue(VerticalScrollBarVisibilityProperty(), value);
    return this;
}

double ScrollViewer::GetHorizontalOffset() const {
    if (scrollContentPresenter_) {
        return scrollContentPresenter_->GetHorizontalOffset();
    }
    return DependencyObject::GetValue<double>(HorizontalOffsetProperty());
}

double ScrollViewer::GetVerticalOffset() const {
    if (scrollContentPresenter_) {
        return scrollContentPresenter_->GetVerticalOffset();
    }
    return DependencyObject::GetValue<double>(VerticalOffsetProperty());
}

bool ScrollViewer::GetCanContentScroll() const {
    return DependencyObject::GetValue<bool>(CanContentScrollProperty());
}

ScrollViewer* ScrollViewer::SetCanContentScroll(bool value) {
    DependencyObject::SetValue(CanContentScrollProperty(), value);
    return this;
}

// ========== 只读属性 ==========

double ScrollViewer::GetExtentWidth() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetExtentWidth() : 0;
}

double ScrollViewer::GetExtentHeight() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetExtentHeight() : 0;
}

double ScrollViewer::GetViewportWidth() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetViewportWidth() : 0;
}

double ScrollViewer::GetViewportHeight() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetViewportHeight() : 0;
}

double ScrollViewer::GetScrollableWidth() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetScrollableWidth() : 0;
}

double ScrollViewer::GetScrollableHeight() const {
    return scrollContentPresenter_ ? scrollContentPresenter_->GetScrollableHeight() : 0;
}

// ========== 滚动操作 ==========

void ScrollViewer::LineUp() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->LineUp();
    }
}

void ScrollViewer::LineDown() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->LineDown();
    }
}

void ScrollViewer::LineLeft() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->LineLeft();
    }
}

void ScrollViewer::LineRight() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->LineRight();
    }
}

void ScrollViewer::PageUp() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->PageUp();
    }
}

void ScrollViewer::PageDown() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->PageDown();
    }
}

void ScrollViewer::PageLeft() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->PageLeft();
    }
}

void ScrollViewer::PageRight() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->PageRight();
    }
}

void ScrollViewer::ScrollToTop() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->ScrollToTop();
    }
}

void ScrollViewer::ScrollToBottom() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->ScrollToBottom();
    }
}

void ScrollViewer::ScrollToLeftEnd() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->ScrollToLeftEnd();
    }
}

void ScrollViewer::ScrollToRightEnd() {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->ScrollToRightEnd();
    }
}

void ScrollViewer::ScrollToHome() {
    ScrollToLeftEnd();
    ScrollToTop();
}

void ScrollViewer::ScrollToEnd() {
    ScrollToRightEnd();
    ScrollToBottom();
}

void ScrollViewer::ScrollToHorizontalOffset(double offset) {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetHorizontalOffset(offset);
    }
}

void ScrollViewer::ScrollToVerticalOffset(double offset) {
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetVerticalOffset(offset);
    }
}

Rect ScrollViewer::MakeVisible(UIElement* /*visual*/, const Rect& rectangle) {
    if (scrollContentPresenter_) {
        return scrollContentPresenter_->MakeVisible(rectangle);
    }
    return rectangle;
}

// ========== 模板应用 ==========

void ScrollViewer::OnTemplateApplied() {
    // 调用基类的 OnTemplateApplied，但由于我们重写了 UpdateContentPresenter 为空操作，
    // 不会发生错误的内容传递
    ContentControl<ScrollViewer>::OnTemplateApplied();
    
    auto* templateRoot = GetTemplateRoot();
    if (!templateRoot) return;
    
    // 查找模板部件
    scrollContentPresenter_ = dynamic_cast<ScrollContentPresenter_t*>(
        templateRoot->FindName(PART_ScrollContentPresenter));
    verticalScrollBar_ = dynamic_cast<ScrollBar*>(
        templateRoot->FindName(PART_VerticalScrollBar));
    horizontalScrollBar_ = dynamic_cast<ScrollBar*>(
        templateRoot->FindName(PART_HorizontalScrollBar));
    
    // 设置 ScrollContentPresenter 的内容
    if (scrollContentPresenter_) {
        // 订阅 ScrollInfoChanged 事件
        scrollContentPresenter_->ScrollInfoChanged += [this]() {
            OnScrollInfoChanged();
        };
        
        // 如果已有内容，设置到 presenter
        auto content = GetContent();
        if (content.has_value()) {
            scrollContentPresenter_->SetContent(content);
            // 强制重新测量以确保内容正确显示
            scrollContentPresenter_->InvalidateMeasure();
        }
        
        // 同步滚动设置
        SyncScrollContentPresenter();
    }
    
    // 绑定垂直滚动条
    if (verticalScrollBar_) {
        verticalScrollBar_->ValueChanged += [this](float oldVal, float newVal) {
            OnVerticalScrollBarValueChanged(oldVal, newVal);
        };
    }
    
    // 绑定水平滚动条
    if (horizontalScrollBar_) {
        horizontalScrollBar_->ValueChanged += [this](float oldVal, float newVal) {
            OnHorizontalScrollBarValueChanged(oldVal, newVal);
        };
    }
    
    // 初始化滚动条可见性和属性
    UpdateScrollBarVisibility();
    SyncScrollBars();
    
    // 强制重新测量整个控件以确保初始布局正确
    InvalidateMeasure();
}

// ========== 内容变化 ==========

void ScrollViewer::OnContentChanged(const std::any& oldContent, const std::any& newContent) {
    ContentControl<ScrollViewer>::OnContentChanged(oldContent, newContent);
    
    // 将内容传递给 ScrollContentPresenter
    if (scrollContentPresenter_) {
        scrollContentPresenter_->SetContent(newContent);
    }
}

// ========== 输入处理 ==========

void ScrollViewer::HandleMouseWheel(float delta) {
    // delta > 0 向上滚动，delta < 0 向下滚动
    if (CanScrollVertically()) {
        if (delta > 0) {
            // 向上滚动多行
            for (int i = 0; i < 3; ++i) {
                LineUp();
            }
        } else if (delta < 0) {
            // 向下滚动多行
            for (int i = 0; i < 3; ++i) {
                LineDown();
            }
        }
    } else if (CanScrollHorizontally()) {
        // 如果不能垂直滚动，尝试水平滚动
        if (delta > 0) {
            for (int i = 0; i < 3; ++i) {
                LineLeft();
            }
        } else if (delta < 0) {
            for (int i = 0; i < 3; ++i) {
                LineRight();
            }
        }
    }
}

void ScrollViewer::OnMouseWheel(PointerEventArgs& e) {
    // 调用基类处理
    ContentControl<ScrollViewer>::OnMouseWheel(e);
    
    // 如果事件已被处理，直接返回
    if (e.handled) {
        return;
    }
    
    // 处理滚轮事件
    // wheelDelta > 0 表示向上滚动，< 0 表示向下滚动
    // 标准增量为 120
    float delta = static_cast<float>(e.wheelDelta) / 120.0f;
    HandleMouseWheel(delta);
    
    // 标记事件已处理
    e.handled = true;
}

// ========== 布局 ==========

Size ScrollViewer::MeasureOverride(const Size& availableSize) {
    // 使用基类测量（会测量模板）
    return ContentControl<ScrollViewer>::MeasureOverride(availableSize);
}

Size ScrollViewer::ArrangeOverride(const Size& finalSize) {
    // 使用基类排列
    Size result = ContentControl<ScrollViewer>::ArrangeOverride(finalSize);
    
    // 排列后更新滚动条（因为此时知道了实际的视口大小）
    UpdateScrollBarVisibility();
    SyncScrollBars();
    
    return result;
}

// ========== 内部方法 ==========

void ScrollViewer::UpdateScrollBarVisibility() {
    if (verticalScrollBar_) {
        bool show = ShouldShowVerticalScrollBar();
        verticalScrollBar_->SetVisibility(show ? Visibility::Visible : Visibility::Collapsed);
    }
    
    if (horizontalScrollBar_) {
        bool show = ShouldShowHorizontalScrollBar();
        horizontalScrollBar_->SetVisibility(show ? Visibility::Visible : Visibility::Collapsed);
    }
}

void ScrollViewer::UpdateContentPresenter() {
    // ScrollViewer 不使用 ContentControl 默认的 ContentPresenter 查找逻辑。
    // 内容的传递由 OnContentChanged 和 OnTemplateApplied 处理，
    // 直接将内容设置到 scrollContentPresenter_。
    // 这里空实现，防止 ContentControl 将内容设置到错误的 ContentPresenter
    // （比如滚动条按钮里的 ContentPresenter）。
}

void ScrollViewer::SyncScrollContentPresenter() {
    if (!scrollContentPresenter_) return;
    
    // 根据 ScrollBarVisibility 设置是否可滚动
    auto hVis = GetHorizontalScrollBarVisibility();
    auto vVis = GetVerticalScrollBarVisibility();
    
    // Disabled 表示完全禁用滚动
    scrollContentPresenter_->SetCanHorizontallyScroll(hVis != ScrollBarVisibility::Disabled);
    scrollContentPresenter_->SetCanVerticallyScroll(vVis != ScrollBarVisibility::Disabled);
}

void ScrollViewer::SyncScrollBars() {
    if (!scrollContentPresenter_) return;
    
    // 同步垂直滚动条
    if (verticalScrollBar_) {
        verticalScrollBar_->SetMinimum(0);
        verticalScrollBar_->SetMaximum(static_cast<float>(GetScrollableHeight()));
        verticalScrollBar_->SetViewportSize(static_cast<float>(GetViewportHeight()));
        
        // 避免循环更新
        if (!isUpdatingFromScrollBar_) {
            isUpdatingFromPresenter_ = true;
            verticalScrollBar_->SetValue(static_cast<float>(GetVerticalOffset()));
            isUpdatingFromPresenter_ = false;
        }
    }
    
    // 同步水平滚动条
    if (horizontalScrollBar_) {
        horizontalScrollBar_->SetMinimum(0);
        horizontalScrollBar_->SetMaximum(static_cast<float>(GetScrollableWidth()));
        horizontalScrollBar_->SetViewportSize(static_cast<float>(GetViewportWidth()));
        
        // 避免循环更新
        if (!isUpdatingFromScrollBar_) {
            isUpdatingFromPresenter_ = true;
            horizontalScrollBar_->SetValue(static_cast<float>(GetHorizontalOffset()));
            isUpdatingFromPresenter_ = false;
        }
    }
}

void ScrollViewer::OnScrollInfoChanged() {
    // ScrollContentPresenter 的滚动信息变化了
    if (isUpdatingFromScrollBar_) return;
    
    // 更新依赖属性（用于绑定）
    DependencyObject::SetValue(HorizontalOffsetProperty(), GetHorizontalOffset());
    DependencyObject::SetValue(VerticalOffsetProperty(), GetVerticalOffset());
    
    // 同步滚动条
    SyncScrollBars();
    
    // 更新滚动条可见性（对于 Auto 模式）
    UpdateScrollBarVisibility();
    
    // 触发事件
    RaiseScrollChanged();
}

void ScrollViewer::OnVerticalScrollBarValueChanged(float /*oldVal*/, float newVal) {
    if (isUpdatingFromPresenter_) return;
    if (!scrollContentPresenter_) return;
    
    isUpdatingFromScrollBar_ = true;
    scrollContentPresenter_->SetVerticalOffset(newVal);
    isUpdatingFromScrollBar_ = false;
}

void ScrollViewer::OnHorizontalScrollBarValueChanged(float /*oldVal*/, float newVal) {
    if (isUpdatingFromPresenter_) return;
    if (!scrollContentPresenter_) return;
    
    isUpdatingFromScrollBar_ = true;
    scrollContentPresenter_->SetHorizontalOffset(newVal);
    isUpdatingFromScrollBar_ = false;
}

void ScrollViewer::RaiseScrollChanged() {
    double hOffset = GetHorizontalOffset();
    double vOffset = GetVerticalOffset();
    
    ScrollChangedEventArgs args(
        hOffset - lastHorizontalOffset_,
        vOffset - lastVerticalOffset_,
        hOffset, vOffset,
        GetExtentWidth(), GetExtentHeight(),
        GetViewportWidth(), GetViewportHeight()
    );
    
    lastHorizontalOffset_ = hOffset;
    lastVerticalOffset_ = vOffset;
    
    ScrollChanged(args);
}

bool ScrollViewer::ShouldShowHorizontalScrollBar() const {
    auto visibility = GetHorizontalScrollBarVisibility();
    
    switch (visibility) {
        case ScrollBarVisibility::Disabled:
        case ScrollBarVisibility::Hidden:
            return false;
        case ScrollBarVisibility::Visible:
            return true;
        case ScrollBarVisibility::Auto:
            // 仅当内容超出视口时显示
            // 但如果viewport还未初始化（=0），暂时不隐藏，等待有效布局信息
            if (GetViewportWidth() <= 0) {
                return true;  // 默认显示，等有效数据后再决定
            }
            return GetScrollableWidth() > 0;
    }
    return false;
}

bool ScrollViewer::ShouldShowVerticalScrollBar() const {
    auto visibility = GetVerticalScrollBarVisibility();
    
    switch (visibility) {
        case ScrollBarVisibility::Disabled:
        case ScrollBarVisibility::Hidden:
            return false;
        case ScrollBarVisibility::Visible:
            return true;
        case ScrollBarVisibility::Auto:
            // 仅当内容超出视口时显示
            // 但如果viewport还未初始化（=0），暂时不隐藏，等待有效布局信息
            if (GetViewportHeight() <= 0) {
                return true;  // 默认显示，等有效数据后再决定
            }
            return GetScrollableHeight() > 0;
    }
    return false;
}

bool ScrollViewer::CanScrollHorizontally() const {
    auto visibility = GetHorizontalScrollBarVisibility();
    return visibility != ScrollBarVisibility::Disabled && GetScrollableWidth() > 0;
}

bool ScrollViewer::CanScrollVertically() const {
    auto visibility = GetVerticalScrollBarVisibility();
    return visibility != ScrollBarVisibility::Disabled && GetScrollableHeight() > 0;
}

} // namespace fk::ui
