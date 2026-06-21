#include "StorageManager.h"

bool StorageManager::begin()
{
    if (!SPIFFS.begin(true))
    {
        Serial.println("[SPIFFS] Mount FAILED");
        return false;
    }

    Serial.println("[SPIFFS] Mounted");

    return true;
}

bool StorageManager::saveTelemetry(const String& json)
{
    File file = SPIFFS.open(fileName, FILE_APPEND);

    if (!file)
    {
        Serial.println("[SPIFFS] Open FAILED");
        return false;
    }

    file.println(json);
    file.close();

    Serial.println("[SPIFFS] Record Saved");

    return true;
}

bool StorageManager::hasPendingData()
{
    File file = SPIFFS.open(fileName);

    if (!file)
        return false;

    bool pending = file.available();

    file.close();

    return pending;
}

String StorageManager::readFirstRecord()
{
    File file = SPIFFS.open(fileName);

    if (!file)
        return "";

    String line = file.readStringUntil('\n');

    file.close();

    return line;
}

bool StorageManager::removeFirstRecord()
{
    SPIFFS.remove(fileName);

    return true;
}

void StorageManager::clear()
{
    SPIFFS.remove(fileName);

    Serial.println("[SPIFFS] Cleared");
}

bool StorageManager::popFirstRecord(String &record)
{
    File file = SPIFFS.open(fileName, FILE_READ);

    if (!file)
        return false;

    if (!file.available())
    {
        file.close();
        return false;
    }

    record = file.readStringUntil('\n');

    String remaining = "";

    while (file.available())
    {
        remaining += file.readStringUntil('\n');
        remaining += "\n";
    }

    file.close();

    file = SPIFFS.open(fileName, FILE_WRITE);

    file.print(remaining);

    file.close();

    return true;
}