#pragma once

#include <mc/ui/input/click_action.hpp>
#include <mc/ui/input/key_modifier.hpp>

namespace mc {
struct KeyClickEvent
{
    int keyCode;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace mc
