#pragma once
#include <ESPAsyncWebServer.h>

namespace WebSocketServer
{
  void init(AsyncWebServer &server);
  void broadcastServoPosition();
  void broadcastCurrentTime();
  bool hasClients();
  void loop();
}