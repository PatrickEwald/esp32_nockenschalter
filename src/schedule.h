#pragma once

namespace Schedule {
  void load();
  void save();
  void checkAndRun();
  bool timeControlEnabled();
  void toggleTimeControl();
  void saveTimeControlStatus();
  void loadTimeControlStatus();
  const int* getTimes();
  const int* getPositions();
}