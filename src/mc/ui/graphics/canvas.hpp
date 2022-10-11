#pragma once

#include <mc/ui/graphics/color_rgba.hpp>

#include <cairo/cairo.h>

namespace mc {

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
    auto fillAll(ColorRGBA color) -> void;

    auto pushState() -> void;
    auto popState() -> void;

    auto native() const -> cairo_t* { return _context; }

private:
    cairo_t* _context{nullptr};
};

}  // namespace mc
