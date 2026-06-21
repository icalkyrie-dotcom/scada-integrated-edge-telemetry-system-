# ADR-002 — Dual Protocol Architecture

## Status

Accepted

---

## Context

Industrial automation systems generally separate operational technology (OT) traffic from information technology (IT) traffic.

SCADA systems require deterministic polling for monitoring and control, while historian and cloud applications require asynchronous telemetry.

Using a single protocol for both purposes introduces unnecessary compromises.

---

## Decision

The ESP32 simultaneously exposes two independent communication paths.

### SCADA Path

ESP32

↓

Modbus TCP

↓

Ignition SCADA

### Historian Path

ESP32

↓

MQTT

↓

Node-RED

↓

InfluxDB

↓

Grafana

Both communication paths operate independently.

---

## Consequences

### Advantages

- SCADA remains operational if MQTT fails.
- Historian continues independently from Modbus polling.
- Clear separation between industrial control and analytics.

### Disadvantages

- Firmware complexity increases.
- Two communication stacks must be maintained.

---

## Validation

The architecture was validated through independent failure testing.

Test performed:

Mosquitto Broker stopped while ESP32 remained online.

Observed result:

- Ignition remained CONNECTED.
- Modbus polling continued normally.
- MQTT telemetry stopped as expected.
- Communication resumed automatically after broker recovery.

This confirms that both communication paths are independent.