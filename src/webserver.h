#include <ESPAsyncWebServer.h>
#pragma once

extern AsyncWebServer server;

void notFound(AsyncWebServerRequest *request); // Optional: für 404

namespace WebServer
{
  void init();
}
