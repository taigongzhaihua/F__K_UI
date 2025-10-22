#pragma once

#include <memory>
#include <string>
#include <utility>

namespace fk {

struct Thickness {
    int left{0};
    int top{0};
    int right{0};
    int bottom{0};

    constexpr Thickness() = default;
    constexpr explicit Thickness(int uniform)
        : left(uniform), top(uniform), right(uniform), bottom(uniform) {}
    constexpr Thickness(int horizontal, int vertical)
        : left(horizontal), top(vertical), right(horizontal), bottom(vertical) {}
    constexpr Thickness(int leftValue, int topValue, int rightValue, int bottomValue)
        : left(leftValue), top(topValue), right(rightValue), bottom(bottomValue) {}
};

class ViewElement {
public:
    struct Size {
        int width{0};
        int height{0};
    };

    struct Rect {
        int x{0};
        int y{0};
        int width{0};
        int height{0};
    };

    virtual ~ViewElement() = default;

    int WidthValue() const noexcept { return width_; }
    void SetWidth(int width) noexcept { width_ = width; }

    int HeightValue() const noexcept { return height_; }
    void SetHeight(int height) noexcept { height_ = height; }

    bool VisibleValue() const noexcept { return visible_; }
    void SetVisible(bool visible) noexcept { visible_ = visible; }

    const Thickness& MarginValue() const noexcept { return margin_; }
    void SetMargin(const Thickness& margin) noexcept { margin_ = margin; }

    const Thickness& PaddingValue() const noexcept { return padding_; }
    void SetPadding(const Thickness& padding) noexcept { padding_ = padding; }

    const std::string& BackgroundValue() const noexcept { return background_; }
    void SetBackground(std::string background) { background_ = std::move(background); }

    virtual void Render() {}

    virtual void Measure(int availableWidth, int availableHeight)
    {
        const int measuredWidth = width_ > 0 ? width_ : availableWidth;
        const int measuredHeight = height_ > 0 ? height_ : availableHeight;
        desiredSize_ = { measuredWidth, measuredHeight };
    }

    virtual void Arrange(int x, int y, int width, int height)
    {
        layoutRect_ = { x, y, width, height };
    }

    const Size& DesiredSize() const noexcept { return desiredSize_; }
    const Rect& LayoutRect() const noexcept { return layoutRect_; }

protected:
    ViewElement() = default;

    void UpdateDesiredSize(int width, int height) noexcept
    {
        desiredSize_ = { width, height };
    }

    void UpdateLayoutRect(int x, int y, int width, int height) noexcept
    {
        layoutRect_ = { x, y, width, height };
    }

private:
    int width_{0};
    int height_{0};
    bool visible_{true};
    Thickness margin_{};
    Thickness padding_{};
    std::string background_{"transparent"};
    Size desiredSize_{};
    Rect layoutRect_{};
};

using ViewElementPtr = std::shared_ptr<ViewElement>;

template <typename Derived>
class View : public ViewElement, public std::enable_shared_from_this<Derived> {
public:
    using Ptr = std::shared_ptr<Derived>;

    Ptr Width(int width)
    {
        SetWidth(width);
        return Self();
    }

    int Width() const noexcept { return WidthValue(); }

    Ptr Height(int height)
    {
        SetHeight(height);
        return Self();
    }

    int Height() const noexcept { return HeightValue(); }

    Ptr IsVisible(bool visible)
    {
        SetVisible(visible);
        return Self();
    }

    bool IsVisible() const noexcept { return VisibleValue(); }

    Ptr Margin(const Thickness& margin)
    {
        SetMargin(margin);
        return Self();
    }

    const Thickness& Margin() const noexcept { return MarginValue(); }

    Ptr Padding(const Thickness& padding)
    {
        SetPadding(padding);
        return Self();
    }

    const Thickness& Padding() const noexcept { return PaddingValue(); }

    Ptr Background(std::string background)
    {
        SetBackground(std::move(background));
        return Self();
    }

    const std::string& Background() const noexcept { return BackgroundValue(); }

    template <typename... Args>
    static Ptr Create(Args&&... args)
    {
        return std::shared_ptr<Derived>(new Derived(std::forward<Args>(args)...));
    }

protected:
    View() = default;

    Ptr Self()
    {
        return std::static_pointer_cast<Derived>(this->shared_from_this());
    }
};

} // namespace fk
