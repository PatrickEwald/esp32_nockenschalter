#include "logging.h"
#include "config.h"
#include <WiFi.h>
#include <time.h>
#include <LittleFS.h>

namespace Log {
  static String buffer;

  void appendToFile(const String& message) {
    File file = LittleFS.open("/log.txt", FILE_APPEND);
    if (!file) return;
    file.println(message);
    file.close();
  }

  void add(const String& msg) {
    // int rssi = WiFi.RSSI();
    // int quality = (rssi <= -100) ? 0 : (rssi >= -50) ? 100 : 2 * (rssi + 100);

    struct tm timeinfo;
    String timeStr = "??:??";
    if (getLocalTime(&timeinfo)) {
      char timeBuffer[6];
      snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
      timeStr = String(timeBuffer);
    }

    String fullMsg = "[" + timeStr + "] " + msg +
                     " | Heap: " + String(ESP.getFreeHeap()); // +
                    //  " B 📶 WLAN: " + String(quality) + "% (" + String(rssi) + " dBm)";
    
    if (buffer.length() + fullMsg.length() > LOG_BUFFER_SIZE) {
      buffer = buffer.substring(fullMsg.length());
    }
    buffer += fullMsg + "\n";
    Serial.println(fullMsg);
    appendToFile(fullMsg); 
  }

  String get() {
    return buffer;
  }

  void clearLogFile() {
    File file = LittleFS.open("/log.txt", FILE_WRITE); 
    if (file) file.close();
    buffer = ""; 
    Log::add("Logdatei geleert.");
  }

}