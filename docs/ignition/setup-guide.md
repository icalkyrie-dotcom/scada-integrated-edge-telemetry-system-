# Ignition SCADA Setup Guide

## Overview

This document describes the configuration required to integrate the ESP32-based IIoT Edge Telemetry System with Ignition SCADA using the Modbus TCP protocol.

---

# Device Configuration

Create a new Modbus TCP Device in Ignition Gateway.

| Parameter | Value |
|-----------|------|
| Device Name | ESP32_RTU_001 |
| Device Type | Modbus TCP |
| IP Address | 192.168.18.87 |
| TCP Port | 502 |
| Unit ID | 1 |
| Connection Timeout | 3000 ms |

After saving the configuration, verify that the device status changes to **CONNECTED**.

---

# Tag Structure

The project uses four logical folders.

```

ESP32\_RTU\_001

├── Sensors
├── Status
├── System
└── Control

```

---

# Sensor Tags

| Tag | OPC Item Path | Data Type | Scaling |
|------|---------------|-----------|---------|
| Temperature | HR1 | Integer | ÷10 |
| Humidity | HR2 | Integer | ÷10 |
| Pressure | HR3 + HR4 | Int32 | ÷100 |

Temperature and humidity are transmitted as scaled INT16 values.

Example:

```

Temperature Register = 269

Actual Temperature = 26.9 °C

```

Humidity follows the same scaling.

---

# Pressure Reconstruction

Pressure is transmitted as a 32-bit integer split into two Holding Registers.

High Word

```

HR3

```

Low Word

```

HR4

```

The original value is reconstructed as

```

Pressure = (High << 16) | Low

```

After reconstruction the value is divided by 100 to obtain pressure in hPa.

Example

```

100358

↓

1003.58 hPa

```

---

# Device Status Register

Device status is represented as a bitmask.

| Bit | Description |
|-----|-------------|
| Bit 0 | Sensor OK |
| Bit 1 | WiFi Connected |
| Bit 2 | MQTT Connected |
| Bit 3 | NTP Synchronized |
| Bit 4 | Modbus Server Active |

Example

```

19

Binary

10011

Sensor OK
WiFi Connected
Modbus Active

```

---

# Alarm Configuration

The dashboard provides operator-adjustable alarm thresholds.

Available controls

- Temperature High Threshold
- Humidity High Threshold

When the threshold is exceeded:

- Alarm Status register is updated.
- Alarm indicator changes state.
- MQTT alarm message is published.
- Ignition alarm visualization updates automatically.

---

# Validation

The following items were verified during system testing.

- Device status remained CONNECTED during continuous polling.
- Temperature values updated in real time.
- Humidity values updated in real time.
- Pressure reconstruction produced correct engineering units.
- Dynamic alarm thresholds operated correctly.
- Modbus communication continued normally when the MQTT broker was offline.

---

# Result

Ignition functions as the SCADA layer of the architecture, independently polling the ESP32 Modbus TCP server while MQTT simultaneously serves the historian and analytics pipeline.