#pragma once
#include "Arduino.h"
#include "config.h"

namespace ServoControl {
    extern int lastPosition;
    inline void init() {}
    inline void move(int position) { lastPosition = position; }
    inline int getCurrentPosition() { return lastPosition; }
    inline String getPositionName(int position) { return ""; }
    inline bool isPushEnabled() { return true; }
    inline void setPushEnabled(bool enabled) {}
}
