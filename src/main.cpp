#include <Arduino.h>

#include "Mpu6050.h"
#include "OrientationFilter.h"
#include "SensorManager.h"
#include "WebServerManager.h"

namespace
{
constexpr char AP_SSID[] = "ESP32-MPU6050";
constexpr char AP_PASSWORD[] = "12345678";

Mpu6050 mpu;
OrientationFilter orientation;
SensorManager sensorManager(mpu, orientation);
WebServerManager webServer(sensorManager);
}

void setup()
{
    Serial.begin(115200);
    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("     ESP32 MPU6050 3D ATTITUDE");
    Serial.println("========================================");

    if (!mpu.begin())
    {
        while (true)
        {
            delay(1000);
        }
    }

    mpu.calibrate();
    orientation.initialize(mpu.readAccelerometer());
    sensorManager.begin();
    webServer.begin(AP_SSID, AP_PASSWORD);

    Serial.println("SYSTEM READY");
    Serial.println("Open http://192.168.4.1");
}

void loop()
{
    delay(1000);
}
