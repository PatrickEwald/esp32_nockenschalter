#include "ota_setup.h"
#include "logging.h"
#include <ArduinoOTA.h>

namespace OTASetup {
  void init() {
    ArduinoOTA.setHostname("ESP32-Servo");
    ArduinoOTA.onStart([]() {
      Log::add("OTA-Update gestartet...");
    });
    ArduinoOTA.onEnd([]() {
      Log::add("OTA-Update abgeschlossen!");
    });
    ArduinoOTA.begin();
    Log::add("OTA initialisiert.");
  }

  void handle() {
    ArduinoOTA.handle();
  }
}