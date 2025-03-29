#include "schedule.h"
#include "config.h"
#include "servo_control.h"
#include "logging.h"
#include <EEPROM.h>
#include <time.h>

namespace Schedule {
  static int timeSchedule[4] = {800, 1200, 1600, 2000};
  static int servoPositions[4] = {AUS, AUS, AUS, AUS};
  static bool enabled = true;
  static unsigned long lastMoveTime = 0;

  void load() {
    for (int i = 0; i < 4; i++) {
      timeSchedule[i] = EEPROM.readInt(i * 4);
      servoPositions[i] = EEPROM.readInt(16 + (i * 4));

      if (servoPositions[i] != SAUNA && servoPositions[i] != AUS && servoPositions[i] != BOILER) {
        servoPositions[i] = AUS;  // Sicherheitswert setzen
      }

      if (timeSchedule[i] < 0 || timeSchedule[i] > 2359) timeSchedule[i] = 800 + (i * 400);
      if (servoPositions[i] != SAUNA && servoPositions[i] != AUS && servoPositions[i] != BOILER) servoPositions[i] = AUS;
    }
    loadTimeControlStatus();
    Log::add("Zeitplan geladen.");
  }

  void save() {
    for (int i = 0; i < 4; i++) {
      EEPROM.writeInt(i * 4, timeSchedule[i]);
      EEPROM.writeInt(16 + (i * 4), servoPositions[i]);
    }
    EEPROM.commit();
    Log::add("Zeitplan gespeichert.");
  }

  int getCurrentTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return 0;
    return timeinfo.tm_hour * 100 + timeinfo.tm_min;
  }

  void checkAndRun() {
    int currentTime = getCurrentTime();
    for (int i = 0; i < 4; i++) {
      if (currentTime == timeSchedule[i] && millis() - lastMoveTime > 60000) {
        ServoControl::move(servoPositions[i]);
        lastMoveTime = millis();
      }
    }
  }

  bool timeControlEnabled() {
    return enabled;
  }

  void toggleTimeControl() {
    enabled = !enabled;
    saveTimeControlStatus();
  }

  void saveTimeControlStatus() {
    EEPROM.writeBool(32, enabled);
    EEPROM.commit();
  }

  void loadTimeControlStatus() {
    uint8_t raw = EEPROM.read(32);
    if (raw != 0 && raw != 1) {
      enabled = true;
      saveTimeControlStatus();
    } else {
      enabled = raw;
    }
  }

  const int* getTimes() {
    return timeSchedule;
  }

  const int* getPositions() {
    return servoPositions;
  }
}