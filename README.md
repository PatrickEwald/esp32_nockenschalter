# ESP32 Nockenschalter

Ein ESP32-basiertes Projekt zur zeitgesteuerten Steuerung eines Nockenschalters per Servo, das per WLAN konfiguriert und überwacht werden kann. Das Projekt verwendet PlatformIO.

## 🔧 Features

- Verbindung zu mehreren WLANs mit `WiFiMulti`
- Zeitplanbasierte Steuerung
- OTA-Updatefähigkeit
- Dateisystem (LittleFS)
- Webinterface
- Automatischer Neustart bei WLAN-Verlust
- Systemgesundheitsprüfung

## 📡 API-Routen

| Route                | Methode | Beschreibung                                       | Parameter                 | Beispiel / Antwort             |
| -------------------- | ------- | -------------------------------------------------- | ------------------------- | ------------------------------ |
| `/servoPos`          | GET     | Gibt die aktuelle Servoposition zurück             | –                         | `Boiler`                       |
| `/moveServo`         | GET     | Bewegt den Servo auf eine neue Position            | `pos=40` \| `94` \| `144` | `Servo bewegt auf Sauna`       |
| `/getSchedule`       | GET     | Gibt den aktuellen Zeitplan als JSON zurück        | –                         | Siehe JSON-Beispiel unten      |
| `/setSchedule`       | GET     | Speichert einen neuen Zeitplan                     | `t1`..`t4`, `p1`..`p4`    | `t1=0700&p1=94`                |
| `/toggleTimeControl` | GET     | Aktiviert oder deaktiviert Zeitsteuerung           | –                         | `Zeitsteuerung aktiviert`      |
| `/getTimeControl`    | GET     | Gibt Status der Zeitsteuerung zurück               | –                         | `aktiviert` oder `deaktiviert` |
| `/togglePush`        | GET     | Aktiviert oder deaktiviert Push-Benachrichtigungen | –                         | `aktiviert` oder `deaktiviert` |
| `/logs`              | GET     | Gibt den aktuellen Logpuffer zurück                | –                         | Textausgabe                    |
| `/logsFile`          | GET     | Gibt den Inhalt der Logdatei `log.txt` zurück      | –                         | Dateiinhalt                    |
| `/clearLog`          | GET     | Löscht die Logdatei                                | –                         | `Logdatei wurde geleert`       |

### 🕒 Beispielantwort für `/getSchedule`

```json
[
  { "time": "0700", "position": "Aus" },
  { "time": "1200", "position": "Boiler" },
  { "time": "1800", "position": "Sauna" },
  { "time": "2200", "position": "Aus" }
]
```
