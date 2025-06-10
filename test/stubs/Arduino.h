#pragma once
#include <stdint.h>
#include <string>

extern unsigned long testMillis;
static inline unsigned long millis() { return testMillis; }

using String = std::string;
