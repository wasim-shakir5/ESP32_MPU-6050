#pragma once

#include <Arduino.h>

struct MpuSample
{
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
};

class Mpu6050
{
public:
    bool begin();
    void calibrate();
    bool read(MpuSample &sample);
    MpuSample readAccelerometer();

    float calibratedAccelX(const MpuSample &sample) const;
    float calibratedAccelY(const MpuSample &sample) const;
    float calibratedAccelZ(const MpuSample &sample) const;
    float calibratedGyroX(const MpuSample &sample) const;
    float calibratedGyroY(const MpuSample &sample) const;

private:
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);

    float accelOffsetX_ = 0.0f;
    float accelOffsetY_ = 0.0f;
    float accelOffsetZ_ = 0.0f;
    float gyroOffsetX_ = 0.0f;
    float gyroOffsetY_ = 0.0f;
    float gyroOffsetZ_ = 0.0f;
};
