#include "canvas.hpp"

namespace mc {
Canvas::ScopedSavedState::ScopedSavedState(Canvas& canvas) : _canvas{canvas} { _canvas.pushState(); }

Canvas::ScopedSavedState::~ScopedSavedState() { _canvas.popState(); }

Canvas::Canvas(cairo_t* context) : _context{context} {}

auto Canvas::fillAll(Color color) -> void
{
    auto const r = redAsFloat(color);
    auto const g = greenAsFloat(color);
    auto const b = blueAsFloat(color);
    cairo_set_source_rgb(_context, r, g, b);
    cairo_set_operator(_context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(_context);
}

auto Canvas::pushState() -> void { cairo_push_group(_context); }

auto Canvas::popState() -> void { cairo_pop_group_to_source(_context); }

}  // namespace mc
