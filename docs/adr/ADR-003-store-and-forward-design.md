# ADR-003 — Store-and-Forward Telemetry Design

## Status

Accepted

---

## Context

Industrial telemetry systems must continue collecting sensor data even when network connectivity is temporarily unavailable.

Without local buffering, telemetry generated during an MQTT outage would be permanently lost.

The objective of this project is to preserve telemetry during communication failures while keeping the implementation lightweight enough for an ESP32.

---

## Decision

A Store-and-Forward mechanism was implemented using the ESP32 SPIFFS filesystem.

When MQTT is unavailable:

1. Sensor telemetry is serialized into JSON.
2. The JSON payload is appended to a local file (`/telemetry.txt`) inside SPIFFS.
3. Data acquisition continues normally.

When MQTT connectivity returns:

1. Stored records are read sequentially.
2. Each record is published to the MQTT telemetry topic.
3. Successfully transmitted records are removed from local storage.
4. Replay continues until no pending records remain.

---

## Consequences

### Advantages

* No telemetry is lost during temporary MQTT outages.
* Sensor acquisition continues independently of network status.
* Lightweight implementation suitable for ESP32 memory constraints.
* Automatic recovery without user intervention.

### Disadvantages

* Replay speed depends on MQTT availability.
* SPIFFS flash memory has finite write endurance.
* Records are stored in a simple text file rather than a database.

---

## Validation

The implementation was validated using the following scenario:

1. MQTT Broker (Mosquitto) was stopped.
2. ESP32 continued collecting sensor data.
3. Telemetry was successfully written into SPIFFS.
4. MQTT Broker was restarted.
5. Stored telemetry was replayed automatically.
6. Grafana displayed the previously buffered data without manual recovery.

This confirms successful Store-and-Forward operation during temporary MQTT outages.
