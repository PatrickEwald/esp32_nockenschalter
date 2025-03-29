#include "button.h"
#include "config.h"
#include "servo_control.h"
#include "logging.h"
#include <Arduino.h>

namespace Button {
  static const int sequence[] = {SAUNA, AUS, BOILER, AUS};
  static const int sequenceLength = sizeof(sequence) / sizeof(sequence[0]);
  static int buttonState = 0;
  static volatile bool pressed = false;
  static unsigned long lastDebounceTime = 0;
  constexpr unsigned long debounceDelay = 500;

  void IRAM_ATTR handleInterrupt() {
    unsigned long currentTime = millis();
    if (currentTime - lastDebounceTime > debounceDelay) {
      pressed = true;
      lastDebounceTime = currentTime;
    }
  }

  void init() {
    pinMode(buttonPin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(buttonPin), handleInterrupt, FALLING);
    Log::add("Button-Interrupt eingerichtet.");
  }

  void check() {
    if (pressed) {
      pressed = false;
      buttonState = (buttonState + 1) % sequenceLength;
      int newPos = sequence[buttonState];
      ServoControl::move(newPos);
      Log::add("Button gedrückt → " + ServoControl::getPositionName(newPos));
    }
  }
}