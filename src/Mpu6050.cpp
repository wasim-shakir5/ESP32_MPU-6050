#include "Mpu6050.h"

#include <Wire.h>

namespace
{
constexpr uint8_t MPU_ADDR = 0x68;
constexpr uint8_t SDA_PIN = 21;
constexpr uint8_t SCL_PIN = 22;
constexpr uint8_t ACCEL_XOUT_H = 0x3B;
constexpr uint8_t PWR_MGMT_1 = 0x6B;
constexpr uint8_t WHO_AM_I = 0x75;
constexpr uint8_t SMPLRT_DIV = 0x19;
constexpr uint8_t CONFIG = 0x1A;
constexpr uint8_t GYRO_CONFIG = 0x1B;
constexpr uint8_t ACCEL_CONFIG = 0x1C;
constexpr float ACCEL_SCALE = 16384.0f;
constexpr float GYRO_SCALE = 131.0f;
constexpr int CALIBRATION_SAMPLES = 1000;
}

bool Mpu6050::begin()
{
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(400000);

    const uint8_t whoAmI = readRegister(WHO_AM_I);
    Serial.printf("WHO_AM_I: 0x%02X\n", whoAmI);
    if (whoAmI != 0x68 && whoAmI != 0x70)
    {
        Serial.println("Unknown sensor.");
        return false;
    }

    writeRegister(PWR_MGMT_1, 0x00);
    delay(100);
    writeRegister(SMPLRT_DIV, 0x09);
    writeRegister(CONFIG, 0x03);
    writeRegister(GYRO_CONFIG, 0x00);
    writeRegister(ACCEL_CONFIG, 0x00);
    delay(100);
    return true;
}

void Mpu6050::writeRegister(uint8_t reg, uint8_t value)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

uint8_t Mpu6050::readRegister(uint8_t reg)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_ADDR, static_cast<uint8_t>(1));
    return Wire.available() ? Wire.read() : 0;
}

bool Mpu6050::read(MpuSample &sample)
{
    Wire.beginTransmission(MPU_ADDR);
    Wire.write(ACCEL_XOUT_H);
    if (Wire.endTransmission(false) != 0 ||
        Wire.requestFrom(MPU_ADDR, static_cast<uint8_t>(14)) != 14)
    {
        return false;
    }

    sample.ax = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    sample.ay = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    sample.az = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    Wire.read();
    Wire.read();
    sample.gx = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    sample.gy = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    sample.gz = static_cast<int16_t>((Wire.read() << 8) | Wire.read());
    return true;
}

MpuSample Mpu6050::readAccelerometer()
{
    MpuSample sample{};
    read(sample);
    return sample;
}

void Mpu6050::calibrate()
{
    Serial.println("Place the MPU6050 flat and do not touch it.");
    delay(3000);

    int64_t sumAX = 0, sumAY = 0, sumAZ = 0;
    int64_t sumGX = 0, sumGY = 0, sumGZ = 0;
    int validSamples = 0;

    for (int sampleIndex = 0; sampleIndex < CALIBRATION_SAMPLES; ++sampleIndex)
    {
        MpuSample sample{};
        if (read(sample))
        {
            sumAX += sample.ax;
            sumAY += sample.ay;
            sumAZ += sample.az;
            sumGX += sample.gx;
            sumGY += sample.gy;
            sumGZ += sample.gz;
            ++validSamples;
        }
        delay(2);
    }

    if (validSamples == 0)
    {
        Serial.println("Calibration failed.");
        return;
    }

    accelOffsetX_ = static_cast<float>(sumAX) / validSamples;
    accelOffsetY_ = static_cast<float>(sumAY) / validSamples;
    accelOffsetZ_ = static_cast<float>(sumAZ) / validSamples - ACCEL_SCALE;
    gyroOffsetX_ = static_cast<float>(sumGX) / validSamples;
    gyroOffsetY_ = static_cast<float>(sumGY) / validSamples;
    gyroOffsetZ_ = static_cast<float>(sumGZ) / validSamples;
    Serial.println("Calibration complete.");
}

float Mpu6050::calibratedAccelX(const MpuSample &sample) const { return (sample.ax - accelOffsetX_) / ACCEL_SCALE; }
float Mpu6050::calibratedAccelY(const MpuSample &sample) const { return (sample.ay - accelOffsetY_) / ACCEL_SCALE; }
float Mpu6050::calibratedAccelZ(const MpuSample &sample) const { return (sample.az - accelOffsetZ_) / ACCEL_SCALE; }
float Mpu6050::calibratedGyroX(const MpuSample &sample) const { return (sample.gx - gyroOffsetX_) / GYRO_SCALE; }
float Mpu6050::calibratedGyroY(const MpuSample &sample) const { return (sample.gy - gyroOffsetY_) / GYRO_SCALE; }
