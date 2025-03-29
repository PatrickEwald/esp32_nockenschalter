#include "filesystem.h"
#include <LittleFS.h>
#include "logging.h"

namespace FileSystem {
  void init() {
    if (!LittleFS.begin(true)) {
      Log::add("❌ Fehler: LittleFS konnte nicht gemountet werden!");
      return;
    }

    File root = LittleFS.open("/");
    while (File file = root.openNextFile()) {
      Log::add("Datei: " + String(file.name()) + " (" + String(file.size()) + " Bytes)");
    }
  }

  void sendFile(AsyncWebServerRequest* request, const String& path, const String& contentType) {
    if (!LittleFS.exists(path)) {
      request->send(404, "text/plain", "Datei nicht gefunden!");
      return;
    }

    request->send(LittleFS, path, contentType);
  }
}