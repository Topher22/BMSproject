#include "../include/BMScontroller.h"

BmsController::BmsController() 
    : currentState(BmsSystemState::PowerUp_SelfTest),
      maxSafeVoltage(4.25f),
      maxSafeTemperature(55.0f),
      activeFaultFlags(0x0000) 
{
    // Complete initialization self-test sequence, transition to nominal
    currentState = BmsSystemState::Contactor_Closed_Nominal;
}

void BmsController::processBatterySensors(const std::vector<BatteryCell>& cellMatrix) {
    // If the system is already latched in Emergency Shutdown, do not recover automatically
    if (currentState == BmsSystemState::EMERGENCY_SHUTDOWN_LATCH) {
        return;
    }

    bool criticalFaultDetected = false;

    for (const auto& cell : cellMatrix) {
        // Check 1: Over-Voltage Evaluation
        if (cell.voltage > maxSafeVoltage) {
            activeFaultFlags |= (1 << 0); // Set Bit 0: Over-Voltage Fault
            criticalFaultDetected = true;
        }
        // Check 2: Over-Temperature Evaluation (Thermal Runaway risk)
        if (cell.temperature > maxSafeTemperature) {
            activeFaultFlags |= (1 << 1); // Set Bit 1: Over-Temperature Fault
            criticalFaultDetected = true;
        }
    }

    // Hard Real-Time Action: Open Safety Contactors if thresholds are violated
    if (criticalFaultDetected) {
        currentState = BmsSystemState::EMERGENCY_SHUTDOWN_LATCH;
        // In real hardware, this instantly triggers a low-level GPIO pin 
        // to cut power to the high-voltage relay coils.
    } else if (currentState == BmsSystemState::Contactor_Closed_Nominal) {
        // Check for minor conditions (e.g., slight warming -> throttle performance)
        for (const auto& cell : cellMatrix) {
            if (cell.temperature > 45.0f) {
                currentState = BmsSystemState::Thermal_Throttling;
                break;
            }
        }
    }
}

CanFdMessage BmsController::generatePowertrainCanFrame() {
    CanFdMessage frame;
    frame.arbitrationId = 0x3B4; // BMW Powertrain Registry ID for BMS Diagnostics
    frame.dataLength = 8;
    
    // Byte 0: State representation token
    frame.payload[0] = static_cast<uint8_t>(currentState);
    
    // Byte 1-2: Active Fault Flag bitmask split across 2 bytes
    frame.payload[1] = static_cast<uint8_t>((activeFaultFlags >> 8) & 0xFF);
    frame.payload[2] = static_cast<uint8_t>(activeFaultFlags & 0xFF);
    
    // Bytes 3-7: Reserved or padded zeros
    for (int i = 3; i < 8; ++i) {
        frame.payload[i] = 0x00;
    }
    
    return frame;
}

void BmsController::resetSafetyLatch() {
    activeFaultFlags = 0x0000;
    currentState = BmsSystemState::Contactor_Closed_Nominal;
}