# MQTT Topic & Payload Schema

## Overview

The ESP32 publishes telemetry and system information through MQTT while simultaneously serving Modbus TCP for SCADA communication.

MQTT is used for historian storage, dashboard visualization, and remote device interaction.

Broker configuration:

| Parameter  | Value         |
| ---------- | ------------- |
| Protocol   | MQTT          |
| Port       | 1883          |
| Keep Alive | 60 seconds    |
| Client ID  | ESP32_RTU_001 |

---

# Topic Overview

| Topic                           | Direction | Description               |
| ------------------------------- | --------- | ------------------------- |
| plant/area1/esp32_001/telemetry | Publish   | Realtime sensor telemetry |
| plant/area1/esp32_001/status    | Publish   | Device status information |
| plant/area1/esp32_001/alarm     | Publish   | Alarm notification        |
| plant/area1/esp32_001/cmd       | Subscribe | Remote command interface  |

---

# Telemetry Topic

Topic

```text
plant/area1/esp32_001/telemetry
```

QoS

```text
0
```

Retained

```text
No
```

Example Payload

```json
{
  "device":"ESP32_RTU_001",
  "temperature":29.13,
  "humidity":66.51,
  "pressure":1007.03,
  "timestamp":1781831371
}
```

Payload Description

| Field       | Type       | Description                |
| ----------- | ---------- | -------------------------- |
| device      | String     | Device identifier          |
| temperature | Float      | Ambient temperature (°C)   |
| humidity    | Float      | Relative humidity (%)      |
| pressure    | Float      | Atmospheric pressure (hPa) |
| timestamp   | Unix Epoch | NTP synchronized timestamp |

---

# Status Topic

Topic

```text
plant/area1/esp32_001/status
```

QoS

```text
0
```

Retained

```text
Yes
```

Example Payload

```json
{
  "device":"ESP32_RTU_001",
  "status":"ONLINE",
  "firmware":256,
  "ip":"192.168.18.xxx",
  "rssi":-49
}
```

Payload Description

| Field    | Type    | Description                |
| -------- | ------- | -------------------------- |
| device   | String  | Device identifier          |
| status   | String  | ONLINE state               |
| firmware | Integer | Firmware version           |
| ip       | String  | Local IP address           |
| rssi     | Integer | WiFi signal strength (dBm) |

---

# Alarm Topic

Topic

```text
plant/area1/esp32_001/alarm
```

QoS

```text
0
```

Retained

```text
No
```

Example Payload

```json
{
  "device":"ESP32_RTU_001",
  "alarm":"TEMP_HIGH",
  "temperature":36.4,
  "humidity":62.1,
  "pressure":1008.4
}
```

Possible Alarm Values

| Alarm         | Description                         |
| ------------- | ----------------------------------- |
| TEMP_HIGH     | High temperature threshold exceeded |
| HUMIDITY_HIGH | High humidity threshold exceeded    |

---

# Command Topic

Topic

```text
plant/area1/esp32_001/cmd
```

Direction

```text
Subscribe
```

Commands are transmitted as JSON objects.

Example

```json
{
    "cmd":"ping"
}
```

Supported Commands

| Command | Action                                      |
| ------- | ------------------------------------------- |
| ping    | Device responds with PONG in serial monitor |
| status  | Republishes device status                   |
| reboot  | Restarts ESP32                              |

---

# Offline Behaviour

When the MQTT broker becomes unavailable:

* Telemetry is automatically stored inside SPIFFS.
* The firmware continues Modbus TCP communication without interruption.
* After reconnection, all buffered telemetry is replayed automatically to the MQTT broker.

This mechanism implements a Store-and-Forward strategy to prevent telemetry loss during temporary network outages.
