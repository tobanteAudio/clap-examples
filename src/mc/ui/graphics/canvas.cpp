#include "canvas.hpp"

namespace mc {
Canvas::ScopedSavedState::ScopedSavedState(Canvas& canvas) : _canvas{canvas} { _canvas.pushState(); }

Canvas::ScopedSavedState::~ScopedSavedState() { _canvas.popState(); }

Canvas::Canvas(cairo_t* context) : _context{context} {}

auto Canvas::setColor(ColorRGBA color) -> void { setColor(toFloat(color)); }

auto Canvas::setColor(ColorFloatRGBA color) -> void
{
    cairo_set_source_rgb(_context, color.red, color.green, color.blue);
}

auto Canvas::fill(ColorRGBA color) -> void
{
    setColor(color);
    cairo_set_operator(_context, CAIRO_OPERATOR_SOURCE);
    cairo_paint(_context);
}

auto Canvas::fillRectangle(Rectangle<int> rect) -> void
{
    cairo_rectangle(_context, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(_context);
}

auto Canvas::pushState() -> void { cairo_push_group(_context); }

auto Canvas::popState() -> void { cairo_pop_group_to_source(_context); }

}  // namespace mc
