#include <iostream>
#include "../include/BMScontroller.h"

int main() {
    std::cout << "=== E-Drive Powertrain Core - Active BMS Booting ===\n";
    BmsController vehicleBms;

    std::vector<BatteryCell> operationalCells = {
        {1, 3.75f, 46.2f}, // Trigger thermal performance throttling state (>45C)
        {2, 3.74f, 42.1f}
    };

    vehicleBms.processBatterySensors(operationalCells);

    if (vehicleBms.getSystemState() == BmsSystemState::Thermal_Throttling) {
        std::cout << "\nℹ️ [BMS NOTICE] Battery warming detected. Entering Performance Throttling Mode.\n";
        CanFdMessage frame = vehicleBms.generatePowertrainCanFrame();
        std::cout << "   [CAN BUS OUT] Sending Frame 0x" << std::hex << frame.arbitrationId 
                  << " -> State Byte: 0x" << static_cast<int>(frame.payload[0]) << std::dec 
                  << " to Inverter to limit max current pull.\n\n";
    }

    return 0;
}