#include <mc/ui.hpp>

auto main() -> int
{
    using namespace mc;

    auto win            = Window{"Application", 1024, 576};
    win.sizeChanged     = [](auto size) { std::printf("Size: %d,%d\n", size.width, size.height); };
    win.positionChanged = [](auto pos) { std::printf("Pos: %d,%d\n", pos.x, pos.y); };
    win.keyClicked      = [](auto event) { std::printf("Key: %c\n", (int)event.keyCode); };
    win.mouseClicked    = [](auto event) { std::printf("Mouse: %d\n", (int)event.button); };
    win.mouseScrolled   = [](auto offset) { std::printf("Scroll: %d,%d\n", offset.x, offset.y); };
    win.mouseMoved      = [](auto pos) { std::printf("Move: %d,%d\n", pos.x, pos.y); };
    win.mouseEnter      = [] { std::puts("Enter"); };
    win.mouseExit       = [] { std::puts("Exit"); };
    win.draw            = [](Canvas& canvas) { canvas.fillAll(Colors::gray); };

    return win.show();
}
