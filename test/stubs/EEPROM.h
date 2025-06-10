#pragma once
#include <cstdint>

class EEPROMClass {
public:
    int readInt(int) { return 0; }
    void writeInt(int, int) {}
    void writeBool(int, bool) {}
    uint8_t read(int) { return 0; }
    void commit() {}
};

extern EEPROMClass EEPROM;
