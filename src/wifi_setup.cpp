#include "wifi_setup.h"
#include "logging.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include "system_health.h"
#include "secrets.h"

namespace WiFiSetup
{

  static WiFiMulti wifiMulti;
  static unsigned long lastCheck = 0;
  static unsigned long reconnectStart = 0;
  static const unsigned long checkInterval = 30000;     // alle 30 Sekunden prüfen
  static const unsigned long reconnectTimeout = 300000; // 5 Minuten bis Neustart

  void init()
  {
    wifiMulti.addAP(WIFI_SSID_1, WIFI_PASSWORD_1);
    wifiMulti.addAP(WIFI_SSID_2, WIFI_PASSWORD_2);

    Log::add("WLAN wird verbunden...");
    while (wifiMulti.run() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Log::add("WLAN verbunden! IP: " + WiFi.localIP().toString());
  }

  void checkConnection()
  {
    unsigned long now = millis();

    if (now - lastCheck < checkInterval)
      return;
    lastCheck = now;

    if (WiFi.status() == WL_CONNECTED)
    {
      reconnectStart = 0; // Verbindung da → Reset
      return;
    }

    checkSystemHealth();

    Log::add("WLAN-Verbindung verloren! Versuche Wiederverbindung...");
    wifiMulti.run(); // Versuch, wieder zu verbinden

    if (reconnectStart == 0)
    {
      reconnectStart = now;
    }
    else if (now - reconnectStart > reconnectTimeout)
    {
      Log::add("WLAN nicht erreichbar. Starte ESP neu...");
      delay(1000);
      ESP.restart();
    }
  }
}