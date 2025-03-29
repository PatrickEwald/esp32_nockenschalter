#include "servo_control.h"
#include "config.h"
#include "logging.h"
#include <ESP32Servo.h>
#include "websocket_server.h"

namespace ServoControl {
  static Servo myServo;
  static int currentPos = AUS;

  void init() {
    myServo.attach(servoPin);
    myServo.write(currentPos);
    Log::add("Servo initialisiert auf: " + getPositionName(currentPos));
  }

  void move(int position) {
  if (position == currentPos) {
    return;
  }

  myServo.write(position);
  currentPos = position;
  Log::add("Servo bewegt zu: " + getPositionName(position));
  WebSocketServer::broadcastServoPosition(); 
  }

  int getCurrentPosition() {
    return currentPos;
  }

String getPositionName(int position) {
  for (int i = 0; i < 3; i++) {
    if (position == POSITIONS[i]) return POSITION_NAMES[i];
  }
  return "Unbekannt";
}
}