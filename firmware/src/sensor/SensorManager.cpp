#include "SensorManager.h"
#include "../config/config.h"

bool SensorManager::begin()
{
    if (!_bme.begin(SENSOR_BME280_ADDR))
    {
        Serial.println("[SENSOR] BME280 not found");

        _initialized = false;

        return false;
    }

    _initialized = true;

    _failCount = 0;

    Serial.println("[SENSOR] BME280 initialized OK");

    return true;
}

bool SensorManager::read(SensorData &data)
{
    if (!_initialized)
    {
        data.valid = false;

        return false;
    }

    data.temperature = _bme.readTemperature();

    data.humidity = _bme.readHumidity();

    data.pressure = _bme.readPressure() / 100.0f;

    data.timestamp_ms = millis();

    if (isnan(data.temperature))
    {
        _failCount++;

        data.valid = false;

        return false;
    }

    _failCount = 0;

    data.valid = true;

    return true;
}

bool SensorManager::isHealthy()
{
    return _initialized && (_failCount < MAX_FAIL);
}