#pragma once
#include <ESPAsyncWebServer.h>

namespace FileSystem {
  void init();
  void sendFile(AsyncWebServerRequest* request, const String& path, const String& contentType);
}