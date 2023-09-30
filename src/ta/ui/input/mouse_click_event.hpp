#pragma once

#include <ta/ui/input/click_action.hpp>
#include <ta/ui/input/key_modifier.hpp>
#include <ta/ui/input/mouse_button.hpp>

namespace ta {
struct MouseClickEvent
{
    MouseButton button;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace ta
