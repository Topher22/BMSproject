# BMSproject
Cell Monitoring &amp; Isolation State Machine

# Real-Time ASIL-D Automotive Battery Management System Safety Controller

A high-integrity, deterministic C++17 embedded simulation framework modeling the cell monitoring telemetry ingestion, fault-containment safety state machine, and powertrain CAN-FD diagnostics broadcasting layers of a generic automotive High-Voltage Battery Management Central Unit (BMCU).

## Core Architectural Objectives & Compliance Matrix
* **ISO 26262 Functional Safety (ASIL-D Model):** Implements a hard real-time fault evaluation loop governing critical Over-Voltage (OV) and Over-Temperature (OT) thresholds with strict fault-containment latching to trip high-voltage contactor systems.
* **MISRA C++:2023 / AUTOSAR Guidelines:** Engineered specifically for memory-constrained embedded safety environments. Zero dynamic heap allocations (`new`/`malloc`) are utilized inside execution paths to eliminate runtime memory fragmentation.
* **Type-Safe Bitwise Serialization:** Emulates low-level CAN Bus frame aggregation using explicit unsigned type safety (`uint8_t`, `uint32_t`) to eliminate undefined implementation behaviors.
* **Decoupled Architecture:** Features separate library, runtime execution application, and automated validation test suite targets via a unified CMake pipeline.

## Compilation & Verification Blueprint
This repository leverages clean, out-of-source builds to isolate tracking architectures from compiled binaries.

```bash
# 1. Initialize CMake build configuration tree
mkdir build && cd build
cmake ..

# 2. Compile both production and regression testing binaries
cmake --build .

# 3. Execute the ASIL-D compliant Software-In-The-Loop (SIL) test suite
./vde_tests

# 4. Launch the live real-time powertrain system demo
./bms_app
