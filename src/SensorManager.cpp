#include "SensorManager.h"

SensorManager::SensorManager(Mpu6050 &mpu, OrientationFilter &orientation)
    : mpu_(mpu), orientation_(orientation)
{
}

void SensorManager::begin()
{
    xTaskCreatePinnedToCore(taskEntry, "SensorTask", 8192, this, 2, nullptr, 1);
}

SensorData SensorManager::snapshot() const
{
    SensorData copy;
    portENTER_CRITICAL(&mux_);
    copy = data_;
    portEXIT_CRITICAL(&mux_);
    return copy;
}

void SensorManager::taskEntry(void *context)
{
    static_cast<SensorManager *>(context)->taskLoop();
}

void SensorManager::taskLoop()
{
    uint32_t previousMicros = micros();
    uint32_t sequence = 0;
    TickType_t lastWake = xTaskGetTickCount();

    while (true)
    {
        const uint32_t now = micros();
        float dt = (now - previousMicros) / 1000000.0f;
        previousMicros = now;
        if (dt <= 0.0f || dt > 0.1f) dt = 0.01f;

        MpuSample sample{};
        if (mpu_.read(sample))
        {
            const Orientation orientation = orientation_.update(mpu_, sample, dt);
            portENTER_CRITICAL(&mux_);
            data_.roll = orientation.roll;
            data_.pitch = orientation.pitch;
            data_.level = orientation.level;
            data_.sequence = ++sequence;
            portEXIT_CRITICAL(&mux_);
        }

        vTaskDelayUntil(&lastWake, pdMS_TO_TICKS(10));
    }
}
