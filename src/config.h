#pragma once

constexpr int servoPin = 13;
constexpr int buttonPin = 14;

constexpr int SAUNA = 144;
constexpr int AUS = 94;
constexpr int BOILER = 40;
constexpr int POSITIONS[3] = {SAUNA, AUS, BOILER};
constexpr const char* POSITION_NAMES[3] = {"Sauna", "Aus", "Boiler"};

constexpr int EEPROM_SIZE = 33;
constexpr int LOG_BUFFER_SIZE = 2048;

constexpr const char* ntpServer = "pool.ntp.org";