#pragma once

#define REG_TEMPERATURE         0
#define REG_HUMIDITY            1

#define REG_PRESSURE_HIGH       2
#define REG_PRESSURE_LOW        3

#define REG_DEVICE_STATUS       4
#define REG_ALARM_STATUS        5

#define REG_UPTIME_HIGH         6
#define REG_UPTIME_LOW          7

#define REG_FIRMWARE_VER        8

#define REG_ALARM_TEMP_HIGH     16
#define REG_ALARM_HUMID_HIGH    17

#define STATUS_SENSOR_OK        (1 << 0)
#define STATUS_WIFI_OK          (1 << 1)
#define STATUS_MQTT_OK          (1 << 2)
#define STATUS_NTP_SYNC         (1 << 3)
#define STATUS_MODBUS_ACTIVE    (1 << 4)

#define ALARM_TEMP_HIGH         (1 << 0)
#define ALARM_HUMIDITY_HIGH     (1 << 1)