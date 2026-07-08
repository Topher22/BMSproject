#pragma once
#include <vector>
#include <string>
#include <cstdint>

// Real-time operational states of a High-Voltage Battery Unit
enum class BmsSystemState {
    PowerUp_SelfTest,
    Contactor_Closed_Nominal,
    Charge_Active,
    Thermal_Throttling,
    EMERGENCY_SHUTDOWN_LATCH,
    Charge_Parameter_Discovery,
    Charge_Current_Demand_Loop
};

// Represents physical cell sensor telemetry acquired via SPI/ADC
struct BatteryCell {
    uint8_t cellId;
    float voltage;       // Nominal: 3.7V, Limit: 4.25V
    float temperature;   // Limit: 55.0°C
};

// Simulated 8-byte packed Automotive CAN Frame structure
struct CanFdMessage {
    uint32_t arbitrationId; // e.g., 0x3B4 for BMS Status
    uint8_t dataLength;
    uint8_t payload[8];
};

class BmsController {
private:
    BmsSystemState currentState;
    float maxSafeVoltage;
    float maxSafeTemperature;
    uint16_t activeFaultFlags; // Bitmask for logging diagnostics (UDS style)
    float targetRequestedCurrent; // Simulated ISO 15118 current target

public:
    BmsController();
    
    // Core hardware loop executed at hard real-time ticks (e.g., every 10ms)
    void processBatterySensors(const std::vector<BatteryCell>& cellMatrix);
    
    // Generates the packed status message to broadcast onto the Powertrain CAN bus
    CanFdMessage generatePowertrainCanFrame();
    
    BmsSystemState getSystemState() const { return currentState; }
    uint16_t getFaultFlags() const { return activeFaultFlags; }
    void resetSafetyLatch();
    void processDcFiledCharging(float chargerMaxVoltage, float chargerMaxCurrent, float currentPackVoltage);
    float getTargetRequestedCurrent() const { return targetRequestedCurrent; }
};