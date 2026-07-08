#include <iostream>
#include <cstdlib>
#include "../include/BMScontroller.h"

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

    // Test Scenario 1: Nominal Cell Ingestion Evaluation
    std::vector<BatteryCell> nominalPack = {
        {1, 3.82f, 25.4f},
        {2, 3.81f, 26.1f},
        {3, 3.83f, 24.9f}
    };
    std::cout << "Executing: TEST-BMS-01 (Nominal Powertrain Environment)\n";
    bms.processBatterySensors(nominalPack);
    verify(bms.getSystemState() == BmsSystemState::Contactor_Closed_Nominal, "System remains fully operational.");

    // Test Scenario 2: Severe Thermal Runaway Mitigation
    std::vector<BatteryCell> dangerousPack = {
        {1, 3.82f, 25.4f},
        {2, 3.81f, 58.7f}, // Cell 2 is overheating dangerously (Limit: 55C)
        {3, 3.83f, 24.9f}
    };
    std::cout << "\nExecuting: TEST-BMS-02 (Thermal Runaway Event Injected)\n";
    bms.processBatterySensors(dangerousPack);
    verify(bms.getSystemState() == BmsSystemState::EMERGENCY_SHUTDOWN_LATCH, "BMS triggered immediate EMERGENCY SHUTDOWN.");
    verify((bms.getFaultFlags() & (1 << 1)) != 0, "Over-Temperature bitmask flagged inside diagnostic registers.");

    // Test Scenario 3: Verify Packed CAN Data Layer Transmission Output
    std::cout << "\nExecuting: TEST-BMS-03 (CAN Bus Packet Composition Evaluation)\n";
    CanFdMessage txFrame = bms.generatePowertrainCanFrame();
    verify(txFrame.arbitrationId == 0x3B4, "Correct CAN arbitration header matched.");
    verify(txFrame.payload[0] == static_cast<uint8_t>(BmsSystemState::EMERGENCY_SHUTDOWN_LATCH), "CAN payload Byte 0 accurately broadcasts safety shutdown state.");

    std::cout << "\n🎉 All hard real-time BMS safety validation loops pass automotive verification!\n\n";
    return 0;
}