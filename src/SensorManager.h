#pragma once

#include <Arduino.h>

#include "Mpu6050.h"
#include "OrientationFilter.h"

struct SensorData
{
    float roll = 0.0f;
    float pitch = 0.0f;
    bool level = false;
    uint32_t sequence = 0;
};

class SensorManager
{
public:
    SensorManager(Mpu6050 &mpu, OrientationFilter &orientation);
    void begin();
    SensorData snapshot() const;

private:
    static void taskEntry(void *context);
    void taskLoop();

    Mpu6050 &mpu_;
    OrientationFilter &orientation_;
    mutable portMUX_TYPE mux_ = portMUX_INITIALIZER_UNLOCKED;
    SensorData data_;
};
