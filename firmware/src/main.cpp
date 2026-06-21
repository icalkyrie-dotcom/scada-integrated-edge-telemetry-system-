#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ModbusTCP.h>
#include <esp_task_wdt.h>

#include "config/config.h"
#include "config/register_map.h"
#include "sensor/SensorManager.h"
#include "mqtt/MQTTManager.h"
#include "storage/StorageManager.h"

MQTTManager mqtt;
SensorManager sensor;
StorageManager storage;
ModbusTCP mb;


uint16_t deviceStatus = 0;
uint16_t alarmStatus = 0;

unsigned long lastRead = 0;

void connectWiFi()
{
    Serial.print("[WIFI] Connecting");

    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("[WIFI] Connected");

    Serial.println();
    Serial.println("===== WIFI INFO =====");

    Serial.print("Mode : ");
    Serial.println(WiFi.getMode());

    Serial.print("Status : ");
    Serial.println(WiFi.status());

    Serial.print("RSSI : ");
    Serial.println(WiFi.RSSI());

    Serial.print("Hostname : ");
    Serial.println(WiFi.getHostname());

    Serial.println("=====================");

    Serial.print("[WIFI] IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WIFI] Gateway    : ");
    Serial.println(WiFi.gatewayIP());

    Serial.print("[WIFI] Subnet     : ");
    Serial.println(WiFi.subnetMask());

    Serial.print("[WIFI] DNS        : ");
    Serial.println(WiFi.dnsIP());

    WiFiClient tcpTest;
    Serial.println();
    Serial.println("========== TCP TEST ==========");

    if (tcpTest.connect(MQTT_BROKER_IP, MQTT_BROKER_PORT))
    {
        Serial.println("TCP CONNECT SUCCESS");
        tcpTest.stop();
    }
    else
    {
        Serial.println("TCP CONNECT FAILED");
    }

    Serial.println("==============================");
}

void syncTime()
{
    Serial.println();
    Serial.println("========== NTP ==========");

    Serial.println("[NTP] Synchronizing...");

    configTime(
        GMT_OFFSET_SEC,
        DAYLIGHT_OFFSET_SEC,
        NTP_SERVER
    );

    struct tm timeinfo;

    while (!getLocalTime(&timeinfo))
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();

    time_t now = time(nullptr);

    Serial.print("[NTP] Epoch : ");
    Serial.println(now);

    Serial.println("[NTP] Synchronization SUCCESS");

    Serial.println("=========================");
}

uint32_t getEpochTime()
{
    return (uint32_t)time(nullptr);
}

void setupModbus()
{
    mb.server(MODBUS_TCP_PORT);

    // Alokasi register sesuai config
    mb.addHreg(0, 0, MODBUS_NUM_REGS);

    Serial.println("[MODBUS] Server Started");
}

void updateModbusRegisters(const SensorData &data)
{
    uint16_t temperature = (uint16_t)(data.temperature * 10);
    uint16_t humidity    = (uint16_t)(data.humidity * 10);

    mb.Hreg(REG_TEMPERATURE, temperature);
    mb.Hreg(REG_HUMIDITY, humidity);

    uint32_t pressure = (uint32_t)(data.pressure * 100);

    mb.Hreg(REG_PRESSURE_HIGH,
            (uint16_t)(pressure >> 16));

    mb.Hreg(REG_PRESSURE_LOW,
            (uint16_t)(pressure & 0xFFFF));
}

void updateSystemRegisters()
{
    mb.Hreg(REG_DEVICE_STATUS, deviceStatus);

    uint32_t uptime = millis() / 1000;

    mb.Hreg(REG_UPTIME_HIGH,
            (uint16_t)(uptime >> 16));

    mb.Hreg(REG_UPTIME_LOW,
            (uint16_t)(uptime & 0xFFFF));

    mb.Hreg(REG_FIRMWARE_VER,
            FIRMWARE_VERSION);
}

void checkAlarms(const SensorData& data)
{
    alarmStatus = 0;

    if (data.temperature > 35.0)
    {
        alarmStatus |= ALARM_TEMP_HIGH;
    }

    if (data.humidity > 80.0)
    {
        alarmStatus |= ALARM_HUMIDITY_HIGH;
    }

    mb.Hreg(REG_ALARM_STATUS, alarmStatus);
}

void setup()
{
    Serial.begin(115200);

    Serial.println();
    Serial.println("==================================");
    Serial.println("IIOT EDGE TELEMETRY SYSTEM");
    Serial.println("Phase 1 - Modbus Validation");
    Serial.println("==================================");

    if (sensor.begin())
    {
        Serial.println("[SYSTEM] SensorManager initialized");
    }
    else
    {
        Serial.println("[SYSTEM] SensorManager FAILED");
    }

    storage.begin();
    connectWiFi();
    syncTime();
    mqtt.begin();

    deviceStatus |= STATUS_WIFI_OK;

    mqtt.publishStatus();

    setupModbus();

    deviceStatus |= STATUS_MODBUS_ACTIVE;

    if (sensor.isHealthy())
    {
        deviceStatus |= STATUS_SENSOR_OK;
    }

    esp_task_wdt_init(30, true);
    esp_task_wdt_add(NULL);
}

void loop()
{
    esp_task_wdt_reset();
    mb.task();
    mqtt.loop();

    if (millis() - lastRead >= 2000)
    {
        lastRead = millis();

        SensorData data;

        if (sensor.read(data))
        {
            updateModbusRegisters(data);

            checkAlarms(data);

            updateSystemRegisters();

            mqtt.publishTelemetry(data);

            mqtt.publishAlarm(data);

            Serial.println();

            Serial.print("Temp: ");
            Serial.print(data.temperature);
            Serial.print(" C");

            Serial.print(" | Hum: ");
            Serial.print(data.humidity);
            Serial.print(" %");

            Serial.print(" | Pressure: ");
            Serial.print(data.pressure);
            Serial.println(" hPa");

            Serial.print("Reg[0] Temp = ");
            Serial.println(mb.Hreg(REG_TEMPERATURE));

            Serial.print("Reg[1] Hum  = ");
            Serial.println(mb.Hreg(REG_HUMIDITY));

            Serial.print("Reg[2] P_H  = ");
            Serial.println(mb.Hreg(REG_PRESSURE_HIGH));

            Serial.print("Reg[3] P_L  = ");
            Serial.println(mb.Hreg(REG_PRESSURE_LOW));

            Serial.print("Status    = ");
            Serial.println(mb.Hreg(REG_DEVICE_STATUS));

            Serial.print("Firmware  = ");
            Serial.println(mb.Hreg(REG_FIRMWARE_VER));

            Serial.print("Alarm     = ");
            Serial.println(mb.Hreg(REG_ALARM_STATUS));

            uint32_t uptime =
                ((uint32_t)mb.Hreg(REG_UPTIME_HIGH) << 16) |
                 mb.Hreg(REG_UPTIME_LOW);
        
            Serial.print("Uptime(s) = ");
            Serial.println(uptime);
            
        }
        else
        {
            Serial.println("[ERROR] Sensor read failed");
        }
    }
}