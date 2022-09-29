#pragma once

#include "color.hpp"
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
    auto fillAll(Color color) -> void;

    auto pushState() -> void;
    auto popState() -> void;

private:
    cairo_t* _context{nullptr};
};

}  // namespace mc
