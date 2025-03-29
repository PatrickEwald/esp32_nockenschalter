#include "websocket_server.h"
#include "servo_control.h"
#include "logging.h"
#include <ESPAsyncWebServer.h>

AsyncWebSocket ws("/ws");
static int lastSentPosition = -1;

void onWebSocketEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len) {
  if (type == WS_EVT_CONNECT) {
    IPAddress newIP = client->remoteIP();
    uint32_t newId = client->id();

    // Durch alle existierenden Clients iterieren
    for (auto c : ws.getClients()) {
      if (c->id() != newId && c->remoteIP() == newIP) {
        Log::add("Doppelte WebSocket-Verbindung erkannt. Alte Verbindung wird geschlossen. ID: " + String(c->id()));
        c->close();
      }
    }
    Log::add("WebSocket verbunden: " + String(newId) + " | Aktive Verbindungen: " + String(ws.count()));
    String pos = ServoControl::getPositionName(ServoControl::getCurrentPosition());
    client->text("{\"servo\":\"" + pos + "\"}");
  } else if (type == WS_EVT_DISCONNECT) {
    Log::add("WebSocket getrennt: " + String(client->id()) + " | Aktive Verbindungen: " + String(ws.count()));
  }
}

void WebSocketServer::init(AsyncWebServer &server) {
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  Log::add("WebSocket initialisiert.");
}

void WebSocketServer::broadcastServoPosition() {
  int current = ServoControl::getCurrentPosition();
  if (current != lastSentPosition) {
    lastSentPosition = current;
    String pos = ServoControl::getPositionName(current);
    ws.textAll("{\"servo\":\"" + pos + "\"}");
    Log::add("WebSocket-Update: " + pos);
  }
}
