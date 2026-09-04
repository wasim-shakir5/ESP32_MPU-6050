#pragma once

#include "Mpu6050.h"

struct Orientation
{
    float roll = 0.0f;
    float pitch = 0.0f;
    bool level = false;
};

class OrientationFilter
{
public:
    void initialize(const MpuSample &sample);
    Orientation update(const Mpu6050 &mpu, const MpuSample &sample, float dt);

private:
    float roll_ = 0.0f;
    float pitch_ = 0.0f;
};
