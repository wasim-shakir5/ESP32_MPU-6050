#include "OrientationFilter.h"

#include <math.h>

namespace
{
constexpr float FILTER_ALPHA = 0.98f;
constexpr float LEVEL_TOLERANCE = 5.0f;
constexpr float RADIANS_TO_DEGREES = 180.0f / PI;
}

void OrientationFilter::initialize(const MpuSample &sample)
{
    const float x = sample.ax / 16384.0f;
    const float y = sample.ay / 16384.0f;
    const float z = sample.az / 16384.0f;
    roll_ = atan2(y, z) * RADIANS_TO_DEGREES;
    pitch_ = atan2(-x, sqrt(y * y + z * z)) * RADIANS_TO_DEGREES;
}

Orientation OrientationFilter::update(const Mpu6050 &mpu, const MpuSample &sample, float dt)
{
    const float ax = mpu.calibratedAccelX(sample);
    const float ay = mpu.calibratedAccelY(sample);
    const float az = mpu.calibratedAccelZ(sample);
    const float accelRoll = atan2(ay, az) * RADIANS_TO_DEGREES;
    const float accelPitch = atan2(-ax, sqrt(ay * ay + az * az)) * RADIANS_TO_DEGREES;

    roll_ = FILTER_ALPHA * (roll_ + mpu.calibratedGyroX(sample) * dt) + (1.0f - FILTER_ALPHA) * accelRoll;
    pitch_ = FILTER_ALPHA * (pitch_ + mpu.calibratedGyroY(sample) * dt) + (1.0f - FILTER_ALPHA) * accelPitch;

    Orientation result;
    result.roll = roll_;
    result.pitch = pitch_;
    result.level = fabs(roll_) <= LEVEL_TOLERANCE && fabs(pitch_) <= LEVEL_TOLERANCE;
    return result;
}
