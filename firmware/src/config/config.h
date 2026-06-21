#pragma once

//==================================================
// Device
//==================================================

#define DEVICE_ID "ESP32_RTU_001"

#define FIRMWARE_VERSION 0x0100

//==================================================
// Sensor
//==================================================

#define SENSOR_READ_INTERVAL_MS 5000
#define SENSOR_BME280_ADDR 0x76

//==================================================
// Modbus
//==================================================

#define MODBUS_TCP_PORT 502
#define MODBUS_UNIT_ID 1
#define MODBUS_NUM_REGS 32

//==================================================
// Alarm
//==================================================

#define ALARM_TEMP_HIGH_DEFAULT 35.0f
#define ALARM_HUMIDITY_HIGH_DEFAULT 80.0f

//==================================================
// Watchdog
//==================================================

#define WDT_TIMEOUT_S 30

//==================================================
// WiFi
//==================================================

#define WIFI_SSID "Falisha"
#define WIFI_PASS "2713302110"

//==================================================
// MQTT
//==================================================

#define MQTT_BROKER_IP "192.168.18.11"
#define MQTT_BROKER_PORT 1883

#define MQTT_CLIENT_ID "ESP32_RTU_001"

#define MQTT_KEEPALIVE_S 60
#define MQTT_RECONNECT_MS 5000

#define MQTT_TOPIC_TELEMETRY "plant/area1/esp32_001/telemetry"
#define MQTT_TOPIC_STATUS    "plant/area1/esp32_001/status"
#define MQTT_TOPIC_ALARM     "plant/area1/esp32_001/alarm"
#define MQTT_TOPIC_CMD       "plant/area1/esp32_001/cmd"

// ===============================
// NTP CONFIG
// ===============================

#define NTP_SERVER          "pool.ntp.org"
#define GMT_OFFSET_SEC      (7 * 3600)      // WIB
#define DAYLIGHT_OFFSET_SEC 0