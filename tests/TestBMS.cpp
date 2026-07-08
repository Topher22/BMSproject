#include <iostream>
#include <cstdlib>
#include "../include/BmsController.h"

void verify(bool assertion, const std::string& checkDescription) {
    if (assertion) {
        std::cout << "  [PASS] " << checkDescription << "\n";
    } else {
        std::cerr << "  [CRITICAL AUTOMOTIVE FAIL] " << checkDescription << "\n";
        std::exit(1);
    }
}

int main() {
    std::cout << "========================================================\n";
    std::cout << "   RUNNING AUTOMOTIVE BMS REVISION TESTS (ASIL-D VAL)  \n";
    std::cout << "========================================================\n";

    BmsController bms;

    // Test Scenario 1: Nominal Environment
    std::vector<BatteryCell> nominalPack = {{1, 3.82f, 25.4f}, {2, 3.81f, 26.1f}};
    std::cout << "Executing: TEST-BMS-01 (Nominal Powertrain Ingestion)\n";
    bms.processBatterySensors(nominalPack);
    verify(bms.getSystemState() == BmsSystemState::Contactor_Closed_Nominal, "System remains functional.");

    // Test Scenario 2: Thermal Runaway Protection
    std::vector<BatteryCell> dangerousPack = {{1, 3.82f, 25.4f}, {2, 3.81f, 58.7f}};
    std::cout << "\nExecuting: TEST-BMS-02 (Thermal Runaway Interlock)\n";
    bms.processBatterySensors(dangerousPack);
    verify(bms.getSystemState() == BmsSystemState::EMERGENCY_SHUTDOWN_LATCH, "BMS triggered immediate Latch state.");
    
    bms.resetSafetyLatch();

    // Test Scenario 3: ISO 15118 Charging Demand Regulation
    std::cout << "\nExecuting: TEST-CHG-01 (Nominal Charging Demand Handshake)\n";
    bms.processDcFiledCharging(450.0f, 250.0f, 350.0f);
    verify(bms.getSystemState() == BmsSystemState::Charge_Current_Demand_Loop, "Transitioned to demand loop.");
    verify(bms.getTargetRequestedCurrent() == 200.0f, "Current capped to vehicle design limits.");

    std::cout << "\n🎉 All automated safety and charging checks passed cleanly!\n\n";
    return 0;
}