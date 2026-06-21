# ADR-001 — Modbus TCP vs OPC UA

## Status

Accepted

---

## Context

The project required a communication protocol that could be implemented on an ESP32 while remaining compatible with industrial SCADA software.

Two common industrial protocols were evaluated:

- Modbus TCP
- OPC UA

Although OPC UA provides richer functionality, it requires significantly more memory, processing power, and implementation complexity than Modbus TCP.

Since the objective of this project is to simulate an industrial Remote Terminal Unit (RTU) running on an ESP32, a lightweight protocol was preferred.

---

## Decision

The firmware implements a Modbus TCP Server using Holding Registers as the primary SCADA communication interface.

The ESP32 acts as a Modbus Slave (Server), while Ignition acts as the Modbus Master (Client).

Sensor values are mapped into Modbus Holding Registers using fixed register addresses.

---

## Consequences

### Advantages

- Very low memory footprint
- Native support in Ignition SCADA
- Simple register-based communication
- Deterministic polling behaviour
- Easy validation using Modbus Poll

### Disadvantages

- Limited data model compared to OPC UA
- No built-in security
- Requires manual scaling and register mapping

---

## Validation

The implementation was validated using:

- Modbus Poll
- Ignition Gateway
- Ignition Perspective

Continuous polling successfully updated all sensor values without communication errors.