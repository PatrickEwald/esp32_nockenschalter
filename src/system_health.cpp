#include "system_health.h"
#include "logging.h"
#include <Arduino.h>

void checkSystemHealth() {
  const int heapThreshold = 10000; // Notfallgrenze in Byte
  int heap = ESP.getFreeHeap();

  if (heap < heapThreshold) {
    Log::add("❗ Notfall: Heap zu niedrig (" + String(heap) + " B) – Neustart in 3 Sekunden!");
    delay(3000);
    Log::add("❗ Neustart wegen niedrigem Heap (" + String(heap) + " B)");
    ESP.restart();
  }
}