#pragma once
#include <Arduino.h>

namespace Log
{
  void add(const String &msg);
  String get();
  void clearLogFile();
}

#define TRACE_STACK() Log::add(String("📍 Trace: ") + __PRETTY_FUNCTION__ + " [" + __FILE__ + ":" + String(__LINE__) + "]")