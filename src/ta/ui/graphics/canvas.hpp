#pragma once

#include <ta/ui/geometry/point_2d.hpp>
#include <ta/ui/geometry/rectangle.hpp>
#include <ta/ui/graphics/color_rgba.hpp>

#include <cairo/cairo.h>

namespace ta {

struct Canvas
{
    struct ScopedSavedState
    {
        explicit ScopedSavedState(Canvas& canvas);
        ~ScopedSavedState();

    private:
        Canvas& _canvas;
    };

    explicit Canvas(cairo_t* context);

    auto setColor(ColorRGBA color) -> void;
    auto setColor(ColorFloatRGBA color) -> void;

    auto fill(ColorRGBA color) -> void;
    auto fillRectangle(Rectangle<int> rect) -> void;

    auto pushState() -> void;
    auto popState() -> void;

    auto native() const -> cairo_t* { return _context; }

private:
    cairo_t* _context{nullptr};
};

}  // namespace ta
