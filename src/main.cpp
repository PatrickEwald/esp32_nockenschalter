
#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"
#include "wifi_setup.h"
#include "ota_setup.h"
#include "filesystem.h"
#include "logging.h"
#include "servo_control.h"
#include "schedule.h"
#include "webserver.h"
#include "websocket_server.h"
#include "button.h"


void setup() {
  Serial.begin(115200);
  delay(500);

  Button::init();
  FileSystem::init();
  EEPROM.begin(EEPROM_SIZE);
  WiFiSetup::init();

  configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", ntpServer);
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Warte auf Zeit vom NTP-Server...");
    delay(1000);
  }

  OTASetup::init();
  Schedule::load();
  ServoControl::init();
  WebSocketServer::init(server);
  WebServer::init();

  Log::add("Setup abgeschlossen.");
}

void loop() {
  OTASetup::handle();
  if (Schedule::timeControlEnabled()) {
    Schedule::checkAndRun();
  }
  Button::check();
}