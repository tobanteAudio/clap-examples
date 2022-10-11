#pragma once

#include "input/click_action.hpp"
#include "input/key_modifier.hpp"
#include "input/mouse_button.hpp"

namespace mc {
struct MouseClickEvent
{
    MouseButton button;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace mc
