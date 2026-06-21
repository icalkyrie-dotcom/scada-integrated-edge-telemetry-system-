#pragma once

#include <WiFiClient.h>
#include <PubSubClient.h>

#include "../sensor/SensorManager.h"
#include "../storage/StorageManager.h"

class MQTTManager
{
public:

    MQTTManager();

    bool begin();

    void loop();

    bool connect();

    bool reconnect();

    bool isConnected();

    bool publishTelemetry(const SensorData& data);

    bool publishStatus();

    void processCommand(const String& cmd);

    void replayStoredData();

    bool publishAlarm(const SensorData& data);

    static void callback(char* topic, byte* payload, unsigned int length);

private:

    WiFiClient wifiClient;

    PubSubClient client;

    StorageManager storage;

    unsigned long lastReconnectAttempt = 0;

    static MQTTManager* instance;
};