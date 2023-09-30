#pragma once

#include <ta/ui/input/click_action.hpp>
#include <ta/ui/input/key_modifier.hpp>

namespace ta {
struct KeyClickEvent
{
    int keyCode;
    ClickAction action;
    KeyModifier modifier;
};
}  // namespace ta
