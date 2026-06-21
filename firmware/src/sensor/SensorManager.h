#pragma once

#include <Adafruit_BME280.h>

struct SensorData
{
    float temperature;
    float humidity;
    float pressure;

    bool valid;

    uint32_t timestamp_ms;
};

class SensorManager
{
public:
    bool begin();

    bool read(SensorData &data);

    bool isHealthy();

private:
    Adafruit_BME280 _bme;

    bool _initialized = false;

    uint8_t _failCount = 0;

    static const uint8_t MAX_FAIL = 3;
};