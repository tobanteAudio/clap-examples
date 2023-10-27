#include <ta/ui.hpp>

auto main() -> int
{
    using namespace ta;

    auto win            = Window{"Application", 1024, 576};
    win.sizeChanged     = [](auto w, auto h) { std::printf("Size: %d,%d\n", w, h); };
    win.positionChanged = [](auto pos) { std::printf("Pos: %d,%d\n", pos.x, pos.y); };
    win.keyClicked      = [](auto event) { std::printf("Key: %c\n", (int)event.keyCode); };
    win.mouseClicked    = [](auto event) { std::printf("Mouse: %d\n", (int)event.button); };
    win.mouseScrolled   = [](auto offset) { std::printf("Scroll: %d,%d\n", offset.x, offset.y); };
    win.mouseMoved      = [](auto pos) { std::printf("Move: %d,%d\n", pos.x, pos.y); };
    win.mouseEnter      = [] { std::puts("Enter"); };
    win.mouseExit       = [] { std::puts("Exit"); };
    win.draw            = [](Canvas& canvas) {
        canvas.fill(Colors::black);

        for (auto i : {0, 1, 2, 3, 4, 5}) {
            canvas.setColor(Colors::green);
            auto* ctx = canvas.native();
            cairo_move_to(ctx, 300.0, 60.0 * i + 40.0);
            cairo_line_to(ctx, 350.0, 60.0 * i + 40.0);
            cairo_set_line_width(ctx, 40.0);
            cairo_set_line_cap(ctx, CAIRO_LINE_CAP_ROUND);
            cairo_stroke(ctx);

            canvas.setColor(ColorRGBA{60, 60, 60});
            canvas.fillRectangle({0, 60 * i + 20, 300, 40});
        }
    };

    return win.show();
}
