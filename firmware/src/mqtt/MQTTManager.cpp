#include <ArduinoJson.h>
#include <WiFi.h>
#include <time.h>

#include "MQTTManager.h"
#include "../config/config.h"

MQTTManager* MQTTManager::instance = nullptr;

MQTTManager::MQTTManager()
    : client(wifiClient)
{
    instance = this;
}

bool MQTTManager::begin()
{
    storage.begin();

    client.setServer(
        MQTT_BROKER_IP,
        MQTT_BROKER_PORT
    );

    client.setKeepAlive(MQTT_KEEPALIVE_S);
    client.setCallback(callback);

    return connect();
}

bool MQTTManager::connect()
{
    Serial.println();
    Serial.println("========== MQTT DEBUG ==========");

    Serial.print("[MQTT] Broker IP   : ");
    Serial.println(MQTT_BROKER_IP);

    Serial.print("[MQTT] Broker Port : ");
    Serial.println(MQTT_BROKER_PORT);

    Serial.print("[MQTT] Client ID   : ");
    Serial.println(MQTT_CLIENT_ID);

    // ==================================================
    // RAW TCP TEST
    // ==================================================

    WiFiClient tcpTest;

    Serial.println();
    Serial.println("[TCP] Testing connection...");

    bool tcpOK = tcpTest.connect(
        MQTT_BROKER_IP,
        MQTT_BROKER_PORT
    );

    if (tcpOK)
    {
        Serial.println("[TCP] CONNECT SUCCESS");
    }
    else
    {
        Serial.println("[TCP] CONNECT FAILED");
    }

    tcpTest.stop();

    // ==================================================
    // MQTT CONNECT
    // ==================================================

    Serial.println();
    Serial.println("[MQTT] Connecting...");

    bool ok = client.connect(MQTT_CLIENT_ID);

    if (ok)
    {
        Serial.println("[MQTT] CONNECTED");

        client.subscribe(MQTT_TOPIC_CMD);

        Serial.print("[MQTT] Subscribed : ");
        Serial.println(MQTT_TOPIC_CMD);

        publishStatus();

        replayStoredData();
    }

    Serial.println("================================");
    Serial.println();

    return ok;
}

bool MQTTManager::reconnect()
{
    if (client.connected())
    {
        return true;
    }

    unsigned long now = millis();

    if (now - lastReconnectAttempt < MQTT_RECONNECT_MS)
    {
        return false;
    }

    lastReconnectAttempt = now;

    return connect();
}

void MQTTManager::loop()
{
    reconnect();

    client.loop();
}

bool MQTTManager::isConnected()
{
    return client.connected();
}

bool MQTTManager::publishTelemetry(const SensorData& data)
{
    JsonDocument doc;

    doc["device"] = DEVICE_ID;
    doc["temperature"] = data.temperature;
    doc["humidity"] = data.humidity;
    doc["pressure"] = data.pressure;
    doc["timestamp"] = (uint32_t)time(nullptr);

    char payload[256];

    serializeJson(doc, payload);

    //--------------------------------------------------
    // MQTT ONLINE
    //--------------------------------------------------

    if (client.connected())
    {
        bool ok = client.publish(
            MQTT_TOPIC_TELEMETRY,
            payload
        );

        if (ok)
        {
            Serial.println("[MQTT] Telemetry Published");
            Serial.println(payload);
        }
        else
        {
            Serial.println("[MQTT] Publish FAILED");

            storage.saveTelemetry(payload);

            Serial.println("[MQTT] Saved to SPIFFS");
        }

        return ok;
    }

    //--------------------------------------------------
    // MQTT OFFLINE
    //--------------------------------------------------

    Serial.println("[MQTT] Offline -> Saving");

    storage.saveTelemetry(payload);

    Serial.println("[MQTT] Saved to SPIFFS");

    return false;
}

bool MQTTManager::publishStatus()
{
    if (!client.connected())
    {
        return false;
    }

    JsonDocument doc;

    doc["device"] = DEVICE_ID;
    doc["status"] = "ONLINE";
    doc["firmware"] = FIRMWARE_VERSION;
    doc["ip"] = WiFi.localIP().toString();
    doc["rssi"] = WiFi.RSSI();

    char payload[256];

    serializeJson(doc, payload);

    bool ok = client.publish(
        MQTT_TOPIC_STATUS,
        payload,
        true          // retained message
    );

    if (ok)
    {
        Serial.println("[MQTT] Status Published");
        Serial.println(payload);
    }
    else
    {
        Serial.println("[MQTT] Status FAILED");
    }

    return ok;
}

bool MQTTManager::publishAlarm(const SensorData& data)
{
    if (!client.connected())
    {
        return false;
    }

    bool alarm = false;

    JsonDocument doc;

    doc["device"] = DEVICE_ID;

    if (data.temperature > ALARM_TEMP_HIGH_DEFAULT)
    {
        doc["alarm"] = "TEMP_HIGH";
        alarm = true;
    }

    if (data.humidity > ALARM_HUMIDITY_HIGH_DEFAULT)
    {
        doc["alarm"] = "HUMIDITY_HIGH";
        alarm = true;
    }

    if (!alarm)
    {
        return false;
    }

    doc["temperature"] = data.temperature;
    doc["humidity"] = data.humidity;
    doc["pressure"] = data.pressure;

    char payload[256];

    serializeJson(doc, payload);

    bool ok = client.publish(
        MQTT_TOPIC_ALARM,
        payload
    );

    if (ok)
    {
        Serial.println("[MQTT] ALARM Published");
        Serial.println(payload);
    }
    else
    {
        Serial.println("[MQTT] ALARM Publish FAILED");
    }

    return ok;
}

void MQTTManager::callback(char* topic, byte* payload, unsigned int length)
{
    Serial.println();
    Serial.println("===== MQTT COMMAND =====");

    Serial.print("Topic : ");
    Serial.println(topic);

    String json = "";

    for (unsigned int i = 0; i < length; i++)
    {
        json += (char)payload[i];
    }

    Serial.print("Payload : ");
    Serial.println(json);

    JsonDocument doc;

    DeserializationError err = deserializeJson(doc, json);

    if (err)
    {
        Serial.println("[CMD] Invalid JSON");
        Serial.println("========================");
        return;
    }

    String cmd = doc["cmd"] | "";

    if (instance != nullptr)
    {
        instance->processCommand(cmd);
    }

    Serial.println("========================");
}

void MQTTManager::processCommand(const String& cmd)
{
    if (cmd == "ping")
    {
        Serial.println("[CMD] PONG");
    }
    else if (cmd == "status")
    {
        Serial.println("[CMD] Publish Status");
        publishStatus();
    }
    else if (cmd == "reboot")
    {
        Serial.println("[CMD] Rebooting...");
        delay(1000);
        ESP.restart();
    }
    else
    {
        Serial.print("[CMD] Unknown Command : ");
        Serial.println(cmd);
    }
}

void MQTTManager::replayStoredData()
{
    if (!client.connected())
        return;

    if (!storage.hasPendingData())
    {
        Serial.println("[SPIFFS] No Pending Data");
        return;
    }

    Serial.println();
    Serial.println("===== REPLAY START =====");

    String record;

    while (storage.popFirstRecord(record))
    {
        bool ok = client.publish(
            MQTT_TOPIC_TELEMETRY,
            record.c_str()
        );

        if (ok)
        {
            Serial.println("[Replay] Sent");
            Serial.println(record);
        }
        else
        {
            Serial.println("[Replay] FAILED");

            storage.saveTelemetry(record);

            break;
        }

        delay(100);
    }

    Serial.println("===== REPLAY DONE =====");
}