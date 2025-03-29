#pragma once
#include <Arduino.h>

namespace Log {
  void add(const String& msg);
  String get();
  void clearLogFile();
}
