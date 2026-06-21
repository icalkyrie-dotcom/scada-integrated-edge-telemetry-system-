#pragma once

#include <Arduino.h>
#include <SPIFFS.h>

class StorageManager
{
public:

    bool begin();

    bool saveTelemetry(const String& json);

    bool hasPendingData();

    String readFirstRecord();

    bool removeFirstRecord();

    bool popFirstRecord(String &record);

    void clear();

private:

    const char* fileName = "/telemetry.txt";
};