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
                      size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    uint32_t id = client->id();
    Log::add("WebSocket verbunden: " + String(id) + " | Aktive Verbindungen: " + String(ws.count()));

    String pos = ServoControl::getPositionName(ServoControl::getCurrentPosition());
    client->text("{\"servo\":\"" + pos + "\"}");
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Log::add("WebSocket getrennt: " + String(client->id()) + " | Aktive Verbindungen: " + String(ws.count()));
  }
}

void WebSocketServer::init(AsyncWebServer &server)
{
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);
  Log::add("WebSocket initialisiert.");
}

void WebSocketServer::broadcastServoPosition()
{
  int current = ServoControl::getCurrentPosition();
  if (current != lastSentPosition)
  {
    lastSentPosition = current;
    String pos = ServoControl::getPositionName(current);
    ws.textAll("{\"servo\":\"" + pos + "\"}");
    Log::add("WebSocket-Update: " + pos);
  }
}

void WebSocketServer::broadcastCurrentTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Log::add("Fehler beim Abrufen der lokalen Zeit");
    return;
  }

  char timeString[9];
  snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d",
           timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  ws.textAll("{\"time\":\"" + String(timeString) + "\"}");
}

bool WebSocketServer::hasClients()
{
  return ws.count() > 0;
}

void WebSocketServer::loop()
{
  // kein Close-Handling mehr notwendig
}