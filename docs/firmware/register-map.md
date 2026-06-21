# Modbus Register Map

## Overview

This document describes the Modbus Holding Register layout implemented by the ESP32-based Industrial IoT Edge Telemetry System.

The register map provides realtime process values, device status information, alarm status, firmware information, and configurable alarm thresholds for SCADA integration through Modbus TCP.

---

## Holding Register Map

| Address | Register Name                    | Data Type | Access       | Scaling   | Description                          |
| ------: | -------------------------------- | --------- | ------------ | --------- | ------------------------------------ |
|       0 | Temperature                      | INT16     | Read Only    | ÷10       | Ambient temperature (°C)             |
|       1 | Humidity                         | INT16     | Read Only    | ÷10       | Relative humidity (%)                |
|       2 | Pressure High Word               | INT16     | Read Only    | High Word | High 16-bit of atmospheric pressure  |
|       3 | Pressure Low Word                | INT16     | Read Only    | Low Word  | Low 16-bit of atmospheric pressure   |
|       4 | Device Status                    | UINT16    | Read Only    | Bitmask   | Device operational status flags      |
|       5 | Alarm Status                     | UINT16    | Read Only    | Bitmask   | Active alarm flags                   |
|       6 | Uptime High Word                 | UINT16    | Read Only    | High Word | High 16-bit of device uptime         |
|       7 | Uptime Low Word                  | UINT16    | Read Only    | Low Word  | Low 16-bit of device uptime          |
|       8 | Firmware Version                 | UINT16    | Read Only    | Integer   | Firmware version identifier          |
|      16 | High Temperature Alarm Threshold | INT16     | Read / Write | ÷10       | High temperature alarm setpoint (°C) |
|      17 | High Humidity Alarm Threshold    | INT16     | Read / Write | ÷10       | High humidity alarm setpoint (%)     |

---

## Device Status Bit Mapping

| Bit | Name          | Description                 |
| --: | ------------- | --------------------------- |
|   0 | SENSOR_OK     | Sensor operating normally   |
|   1 | WIFI_OK       | WiFi connection established |
|   2 | MQTT_OK       | MQTT broker connected       |
|   3 | NTP_SYNC      | Time synchronized via NTP   |
|   4 | MODBUS_ACTIVE | Modbus TCP service active   |

---

## Alarm Status Bit Mapping

| Bit | Name          | Description                   |
| --: | ------------- | ----------------------------- |
|   0 | TEMP_HIGH     | High temperature alarm active |
|   1 | HUMIDITY_HIGH | High humidity alarm active    |

---

## 32-bit Register Reconstruction

Pressure and Uptime are stored using two consecutive 16-bit Holding Registers.

To reconstruct the original 32-bit value:

```
Value = (HighWord << 16) | LowWord
```

---

## Scaling Convention

| Measurement | Scaling                          |
| ----------- | -------------------------------- |
| Temperature | Register Value ÷ 10              |
| Humidity    | Register Value ÷ 10              |
| Pressure    | Reconstructed 32-bit value ÷ 100 |

---

## Notes

* Holding Registers 9–15 are currently reserved for future expansion.
* Registers 16 and 17 are writable from the SCADA system to allow runtime alarm threshold configuration.
* Pressure and Uptime require 32-bit reconstruction before conversion to engineering units.
* Device Status and Alarm Status are represented as bitmask registers.
