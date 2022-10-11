#include "window.hpp"

auto main() -> int
{
    using namespace mc;

    auto win            = Window{"Application", 1024, 576};
    win.sizeChanged     = [](int w, int h) { std::printf("Size: %d,%d\n", w, h); };
    win.positionChanged = [](int w, int h) { std::printf("Pos: %d,%d\n", w, h); };
    win.keyClicked      = [](auto event) { std::printf("Key: %c\n", (int)event.keyCode); };
    win.mouseClicked    = [](auto event) { std::printf("Mouse: %d\n", (int)event.button); };
    win.mouseScrolled   = [](auto off) { std::printf("Scroll: %f,%f\n", off.x, off.y); };
    win.mouseMoved      = [](auto pos) { std::printf("Move: %f,%f\n", pos.x, pos.y); };
    win.mouseEnter      = []() { std::puts("Enter"); };
    win.mouseExit       = []() { std::puts("Exit"); };
    win.draw            = [](Canvas& canvas) { canvas.fillAll(Colors::gray); };

    return win.show();
}
