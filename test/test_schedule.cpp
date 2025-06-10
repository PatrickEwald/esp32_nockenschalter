#include <unity.h>
#include "schedule.h"
#include "config.h"
#include "servo_control.h"
#include <time.h>

unsigned long testMillis = 0;
int ServoControl::lastPosition = -1;
EEPROMClass EEPROM; // from stub

static struct tm fakeTime;

extern "C" bool getLocalTime(struct tm *info)
{
    if(!info) return false;
    *info = fakeTime;
    return true;
}

void setFakeTime(int hh, int mm)
{
    fakeTime = {};
    fakeTime.tm_hour = hh;
    fakeTime.tm_min = mm;
}

void test_schedule_runs_and_moves_servo()
{
    // prepare schedule
    int *times = const_cast<int *>(Schedule::getTimes());
    int *positions = const_cast<int *>(Schedule::getPositions());
    times[0] = 1000;      // 10:00
    positions[0] = SAUNA;
    times[1] = 1200; positions[1] = AUS;
    times[2] = 1600; positions[2] = AUS;
    times[3] = 2000; positions[3] = AUS;

    ServoControl::lastPosition = -1;
    testMillis = 61000;  // ensure enough time passed
    setFakeTime(10, 0);

    Schedule::checkAndRun();

    TEST_ASSERT_EQUAL(SAUNA, ServoControl::lastPosition);
}

int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_schedule_runs_and_moves_servo);
    return UNITY_END();
}
