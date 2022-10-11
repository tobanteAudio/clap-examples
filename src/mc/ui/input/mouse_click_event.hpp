#pragma once

#include <mc/ui/input/click_action.hpp>
#include <mc/ui/input/key_modifier.hpp>
#include <mc/ui/input/mouse_button.hpp>

namespace mc {
struct MouseClickEvent
{
    MouseButton button;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace mc
