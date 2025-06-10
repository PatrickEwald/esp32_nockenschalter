#include <ESPAsyncWebServer.h>
#pragma once

extern AsyncWebServer server;

void notFound(AsyncWebServerRequest *request); // Handler für 404-Fehlerseiten

namespace WebServer
{
  void init();
}