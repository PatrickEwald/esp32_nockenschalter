#include "config.h"
#include "webserver.h"
#include "servo_control.h"
#include "schedule.h"
#include "logging.h"
#include "filesystem.h"
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// ===== Hilfsfunktionen =====

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Datei nicht gefunden");
}

String buildScheduleJson() {
  String json = "[";
  auto times = Schedule::getTimes();
  auto positions = Schedule::getPositions();

  for (int i = 0; i < 4; i++) {
    char timeBuf[5];
    snprintf(timeBuf, sizeof(timeBuf), "%04d", times[i]);
    json += "{\"time\":\"" + String(timeBuf) + "\",\"position\":\"" +
            ServoControl::getPositionName(positions[i]) + "\"}";
    if (i < 3) json += ",";
  }
  json += "]";
  return json;
}

// ===== Routengruppen =====

void setupStaticRoutes() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    FileSystem::sendFile(request, "/index.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    FileSystem::sendFile(request, "/style.css", "text/css");
  });

  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    FileSystem::sendFile(request, "/script.js", "application/javascript");
  });

  server.on("/logsFile", HTTP_GET, [](AsyncWebServerRequest *request) {
    FileSystem::sendFile(request, "/log.txt", "text/plain");
  });
}

void setupAPIRoutes() {
  server.on("/getSchedule", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "application/json", buildScheduleJson());
  });

  server.on("/setSchedule", HTTP_GET, [](AsyncWebServerRequest *request) {
    auto times = Schedule::getTimes();
    auto positions = Schedule::getPositions();

    for (int i = 0; i < 4; i++) {
      String tParam = "t" + String(i + 1);
      String pParam = "p" + String(i + 1);

      if (request->hasParam(tParam)) {
        int t = request->getParam(tParam)->value().toInt();
        if (t >= 0 && t <= 2359) ((int*)times)[i] = t;
      }

      if (request->hasParam(pParam)) {
        int p = request->getParam(pParam)->value().toInt();
        if (p == SAUNA || p == AUS || p == BOILER) ((int*)positions)[i] = p;
      }
    }

    Schedule::save();
    request->redirect("/");
  });

  server.on("/toggleTimeControl", HTTP_GET, [](AsyncWebServerRequest *request) {
    Schedule::toggleTimeControl();
    request->send(200, "text/plain", Schedule::timeControlEnabled() ? "Zeitsteuerung aktiviert" : "Zeitsteuerung deaktiviert");
  });

  server.on("/getTimeControl", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", Schedule::timeControlEnabled() ? "aktiviert" : "deaktiviert");
  });

  server.on("/servoPos", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", ServoControl::getPositionName(ServoControl::getCurrentPosition()));
  });

  server.on("/moveServo", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pos")) {
      int pos = request->getParam("pos")->value().toInt();
      ServoControl::move(pos);
      request->send(200, "text/plain", "Servo bewegt auf " + ServoControl::getPositionName(pos));
    } else {
      request->send(400, "text/plain", "Keine Position angegeben");
    }
  });
}

void setupDebugRoutes() {
  server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", Log::get());
  });

  server.on("/clearLog", HTTP_GET, [](AsyncWebServerRequest *request) {
    Log::clearLogFile();
    request->send(200, "text/plain", "Logdatei wurde geleert");
  });
}



namespace WebServer {
  void init() {
    setupStaticRoutes();
    setupAPIRoutes();
    setupDebugRoutes();
    server.onNotFound(notFound);
    server.begin();
    Log::add("Webserver gestartet.");
  }
}