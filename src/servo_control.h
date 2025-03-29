#pragma once
#include <Arduino.h>

namespace ServoControl {
  void init();
  void move(int position);
  int getCurrentPosition();
  String getPositionName(int position);
}