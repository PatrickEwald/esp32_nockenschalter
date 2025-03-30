#include "servo_control.h"
#include "config.h"
#include "secrets.h"
#include "logging.h"
#include <ESP32Servo.h>
#include "websocket_server.h"
#include <Preferences.h>
#include <HTTPClient.h>

static Preferences preferences;

namespace ServoControl
{
  static Servo myServo;
  static int currentPos = AUS;
  static bool pushEnabled = true;

  bool isPushEnabled()
  {
    return pushEnabled;
  }

  void init()
  {
    preferences.begin("servo", false);
    currentPos = preferences.getInt("lastPos", AUS);
    pushEnabled = preferences.getBool("pushEnabled", true);

    myServo.attach(servoPin);
    myServo.write(currentPos);
    Log::add("Servo initialisiert auf: " + getPositionName(currentPos));
  }

  void sendNotification(const String &message)
  {
    if (!pushEnabled)
    {
      return;
    }

    struct tm timeinfo;
    String timeStr = "";

    if (getLocalTime(&timeinfo))
    {
      char buf[20];
      strftime(buf, sizeof(buf), "%H:%M:%S", &timeinfo);
      timeStr = String(buf);
    }
    else
    {
      timeStr = "Zeit unbekannt";
    }

    String fullMessage = "[" + timeStr + "] " + message;

    HTTPClient http;
    http.begin(HA_NOTIFY_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + String(HA_TOKEN));

    String payload = "{\"message\":\"" + fullMessage + "\"}";
    http.POST(payload);
    http.end();
    Log::add("Push-Benachrichtigung gesendet: ");
  }

  void setPushEnabled(bool enabled)
  {
    pushEnabled = enabled;
    preferences.putBool("pushEnabled", enabled);
    Log::add("Push-Benachrichtigungen " + String(enabled ? "aktiviert" : "deaktiviert"));
  }

  void move(int position)
  {
    if (position == currentPos)
    {
      return;
    }

    myServo.write(position);
    currentPos = position;
    preferences.putInt("lastPos", currentPos);
    Log::add("Servo bewegt zu: " + getPositionName(position));
    WebSocketServer::broadcastServoPosition();
    sendNotification("Schalter wurde auf '" + getPositionName(position) + "' gestellt.");
  }

  int getCurrentPosition()
  {
    return currentPos;
  }

  String getPositionName(int position)
  {
    for (int i = 0; i < 3; i++)
    {
      if (position == POSITIONS[i])
        return POSITION_NAMES[i];
    }
    return "Unbekannt";
  }
}