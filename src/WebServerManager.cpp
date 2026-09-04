#include "WebServerManager.h"

#include <WiFi.h>

namespace
{
const char INDEX_HTML[] PROGMEM = R"rawliteral(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Attitude</title>
<style>
* { box-sizing: border-box; }
html, body { margin: 0; width: 100%; height: 100%; background: #000; color: #fff; font-family: Arial, Helvetica, sans-serif; overflow: hidden; }
body { display: flex; justify-content: center; align-items: center; }
.app { width: 100%; max-width: 1100px; height: 100vh; padding: 30px; display: flex; flex-direction: column; }
header { display: flex; justify-content: space-between; align-items: center; border-bottom: 1px solid #222; padding-bottom: 20px; }
.eyebrow { font-size: 11px; letter-spacing: 4px; color: #666; }
h1 { margin: 5px 0 0; font-size: 30px; font-weight: 400; letter-spacing: 8px; }
.connection { display: flex; align-items: center; gap: 8px; font-size: 11px; letter-spacing: 2px; }
.dot { width: 8px; height: 8px; border-radius: 50%; background: #444; }
.connection.online .dot { background: #fff; }
.connection.offline .dot { background: #555; }
.display { flex: 1; min-height: 0; display: flex; flex-direction: column; align-items: center; justify-content: center; }
.plane-container { width: min(70vw, 650px); height: min(50vh, 400px); perspective: 900px; display: flex; align-items: center; justify-content: center; }
.plane { position: relative; width: 320px; height: 210px; transform-style: preserve-3d; transition: transform 70ms linear; }
.fuselage { position: absolute; left: 143px; top: 18px; width: 34px; height: 174px; border: 2px solid #fff; border-radius: 48% 48% 42% 42%; background: linear-gradient(90deg, #050505 0 40%, #202020 50%, #050505 60%); transform: translateZ(24px); }
.nose { position: absolute; left: 145px; top: 0; width: 30px; height: 48px; border: 2px solid #fff; border-bottom: 0; border-radius: 50% 50% 0 0; background: #111; transform: translateZ(28px); }
.cockpit { position: absolute; left: 150px; top: 38px; width: 20px; height: 37px; border: 1px solid #9de8ff; border-radius: 48% 48% 38% 38%; background: linear-gradient(135deg, #d8fbff, #28778f 55%, #07151b); transform: translateZ(31px); }
.wing { position: absolute; top: 72px; width: 165px; height: 72px; border: 2px solid #fff; background: linear-gradient(150deg, #252525, #050505 62%); transform: translateZ(17px); }
.wing-left { left: -8px; clip-path: polygon(0 58%, 100% 0, 100% 34%, 30% 100%, 0 100%); }
.wing-right { right: -8px; clip-path: polygon(0 0, 100% 58%, 100% 100%, 70% 100%, 0 34%); }
.tailplane { position: absolute; top: 143px; width: 82px; height: 42px; border: 2px solid #fff; background: #111; transform: translateZ(18px); }
.tail-left { left: 73px; clip-path: polygon(0 58%, 100% 0, 100% 38%, 34% 100%, 0 100%); }
.tail-right { right: 73px; clip-path: polygon(0 0, 100% 58%, 100% 100%, 66% 100%, 0 38%); }
.fin { position: absolute; left: 151px; top: 127px; width: 26px; height: 64px; border: 2px solid #fff; border-bottom: 0; background: #151515; clip-path: polygon(42% 0, 100% 100%, 0 100%); transform: translateZ(8px) rotateX(-12deg); transform-origin: bottom; }
.engine { position: absolute; top: 107px; width: 22px; height: 42px; border: 2px solid #aaa; border-radius: 45%; background: #111; transform: translateZ(25px); }
.engine-left { left: 74px; }
.engine-right { right: 74px; }
.status { margin-top: 10px; font-size: 18px; letter-spacing: 6px; font-weight: 500; }
.status.green { color: #00ff66; }
.status.red { color: #ff3030; }
.telemetry { display: grid; grid-template-columns: repeat(3, 1fr); border-top: 1px solid #222; border-bottom: 1px solid #222; }
.telemetry-item { padding: 18px; border-right: 1px solid #222; }
.telemetry-item:last-child { border-right: none; }
.label { display: block; color: #555; font-size: 10px; letter-spacing: 3px; margin-bottom: 7px; }
.value { font-family: monospace; font-size: 24px; }
footer { display: flex; justify-content: space-between; padding-top: 15px; color: #444; font-size: 9px; letter-spacing: 2px; }
@media (max-width: 600px) { .app { padding: 15px; } h1 { font-size: 22px; letter-spacing: 5px; } .plane { transform: scale(0.75); } .telemetry-item { padding: 10px; } .value { font-size: 17px; } footer { display: none; } }
</style>
</head>
<body>
<main class="app">
<header><div><div class="eyebrow">ESP32 / MPU6050</div><h1>ATTITUDE</h1></div><div id="connection" class="connection offline"><span class="dot"></span><span id="connectionText">OFFLINE</span></div></header>
<section class="display"><div class="plane-container"><div id="plane" class="plane"><div class="wing wing-left"></div><div class="wing wing-right"></div><div class="engine engine-left"></div><div class="engine engine-right"></div><div class="tailplane tail-left"></div><div class="tailplane tail-right"></div><div class="fin"></div><div class="fuselage"></div><div class="nose"></div><div class="cockpit"></div></div></div><div id="status" class="status red">NOT LEVEL</div></section>
<section class="telemetry"><div class="telemetry-item"><span class="label">ROLL</span><span id="roll" class="value">0.00°</span></div><div class="telemetry-item"><span class="label">PITCH</span><span id="pitch" class="value">0.00°</span></div><div class="telemetry-item"><span class="label">RATE</span><span id="rate" class="value">0 Hz</span></div></section>
<footer><span>MPU6050</span><span>WEBSOCKET / BINARY</span><span>CORE 1 SENSOR</span><span>CORE 0 NETWORK</span></footer>
</main>
<script>
"use strict";
const plane = document.getElementById("plane");
const rollElement = document.getElementById("roll");
const pitchElement = document.getElementById("pitch");
const rateElement = document.getElementById("rate");
const statusElement = document.getElementById("status");
const connectionElement = document.getElementById("connection");
const connectionText = document.getElementById("connectionText");
let packetCount = 0;
let lastRateTime = performance.now();
function connect() {
    const socket = new WebSocket(`ws://${window.location.hostname}:81`);
    socket.binaryType = "arraybuffer";
    socket.onopen = () => { connectionElement.classList.remove("offline"); connectionElement.classList.add("online"); connectionText.textContent = "ONLINE"; };
    socket.onclose = () => { connectionElement.classList.remove("online"); connectionElement.classList.add("offline"); connectionText.textContent = "OFFLINE"; setTimeout(connect, 1000); };
    socket.onerror = error => console.error("WebSocket error", error);
    socket.onmessage = event => {
        if (!(event.data instanceof ArrayBuffer) || event.data.byteLength < 13) return;
        const view = new DataView(event.data);
        const roll = view.getFloat32(0, true);
        const pitch = view.getFloat32(4, true);
        const level = view.getUint8(8);
        plane.style.transform = `rotateX(${-pitch}deg) rotateZ(${-roll}deg)`;
        rollElement.textContent = `${roll.toFixed(2).padStart(6, " ")}°`;
        pitchElement.textContent = `${pitch.toFixed(2).padStart(6, " ")}°`;
        statusElement.classList.toggle("green", level === 1);
        statusElement.classList.toggle("red", level !== 1);
        statusElement.textContent = level === 1 ? "LEVEL" : "NOT LEVEL";
        packetCount++;
    };
}
setInterval(() => { const now = performance.now(); const elapsed = now - lastRateTime; if (elapsed >= 1000) { rateElement.textContent = `${(packetCount / (elapsed / 1000)).toFixed(0)} Hz`; packetCount = 0; lastRateTime = now; } }, 250);
connect();
</script>
</body>
</html>)rawliteral";
}

namespace
{
#pragma pack(push, 1)
struct WebSocketPacket
{
    float roll;
    float pitch;
    uint8_t level;
    uint32_t sequence;
};
#pragma pack(pop)
}

WebServerManager::WebServerManager(const SensorManager &sensorManager)
    : sensorManager_(sensorManager)
{
}

void WebServerManager::begin(const char *ssid, const char *password)
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);

    server_.on("/", HTTP_GET, [this]() { server_.send_P(200, "text/html", INDEX_HTML); });
    server_.onNotFound([this]() { server_.send(404, "text/plain", "Not found"); });
    server_.begin();

    webSocket_.begin();
    webSocket_.onEvent(webSocketEvent);
    xTaskCreatePinnedToCore(taskEntry, "NetworkTask", 8192, this, 1, nullptr, 0);

    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void WebServerManager::taskEntry(void *context)
{
    static_cast<WebServerManager *>(context)->taskLoop();
}

void WebServerManager::taskLoop()
{
    uint32_t lastSend = millis();
    while (true)
    {
        webSocket_.loop();
        server_.handleClient();

        const uint32_t now = millis();
        if (now - lastSend >= 20)
        {
            lastSend = now;
            const SensorData data = sensorManager_.snapshot();
            WebSocketPacket packet{data.roll, data.pitch, static_cast<uint8_t>(data.level ? 1 : 0), data.sequence};
            webSocket_.broadcastBIN(reinterpret_cast<uint8_t *>(&packet), sizeof(packet));
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void WebServerManager::webSocketEvent(uint8_t clientNum, WStype_t type, uint8_t *, size_t)
{
    if (type == WStype_CONNECTED)
    {
        Serial.printf("WebSocket client connected: %u\n", clientNum);
    }
    else if (type == WStype_DISCONNECTED)
    {
        Serial.printf("WebSocket client disconnected: %u\n", clientNum);
    }
}
