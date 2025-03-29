#pragma once
#include <WiFiMulti.h>
#include <Arduino.h>

namespace WiFiSetup {
  void init();                  // Verbindet mit einem der Netzwerke
  String getIP();               // Gibt IP-Adresse als String zurück
  int getRSSI();                // Gibt aktuellen RSSI-Wert zurück
  int getQuality();             // Gibt WLAN-Qualität in % zurück
  void checkConnection();       // Überprüft die Verbindung und stellt sie bei Bedarf wieder her
}