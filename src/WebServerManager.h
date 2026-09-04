#pragma once

#include <WebServer.h>
#include <WebSocketsServer.h>

#include "SensorManager.h"

class WebServerManager
{
public:
    explicit WebServerManager(const SensorManager &sensorManager);
    void begin(const char *ssid, const char *password);

private:
    static void taskEntry(void *context);
    static void webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t *payload, size_t length);
    void taskLoop();

    const SensorManager &sensorManager_;
    WebServer server_{80};
    WebSocketsServer webSocket_{81};
};
