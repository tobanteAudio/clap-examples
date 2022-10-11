#pragma once

#include "input/click_action.hpp"
#include "input/key_modifier.hpp"

namespace mc {
struct KeyClickEvent
{
    int keyCode;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace mc
