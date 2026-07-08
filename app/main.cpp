#include <iostream>
#include <vector>
#include "../include/BMScontroller.h"

int main() {
    std::cout << "========================================================\n";
    std::cout << "   HIGH-VOLTAGE POWERTRAIN CORE - ACTIVE BMS MODULE     \n";
    std::cout << "========================================================\n\n";

    BmsController vehicleBms;

    // 1. Simulate active battery sensor monitoring
    std::vector<BatteryCell> initialPackStatus = {
        {1, 3.75f, 46.2f}, // Cell 1 is warm (>45.0°C), triggering performance throttling
        {2, 3.74f, 42.1f}
    };

    std::cout << "[SYS_TICK] processing cell telemetry matrix data...\n";
    vehicleBms.processBatterySensors(initialPackStatus);

    if (vehicleBms.getSystemState() == BmsSystemState::Thermal_Throttling) {
        std::cout << "  [NOTICE] System entering Performance Throttling Mode due to thermal thresholds.\n";
    }

    // 2. Simulate connecting to an ISO 15118 / DIN SPEC 70121 DC Charging Station
    std::cout << "\n[EVSE_LINK] Charging cable coupled. Initiating DC charging loop...\n";
    
    // Parameters: Charger Max 450V, Charger Max 250A, Current Vehicle Pack 350V
    vehicleBms.processDcFiledCharging(450.0f, 250.0f, 350.0f);

    if (vehicleBms.getSystemState() == BmsSystemState::Charge_Current_Demand_Loop) {
        std::cout << "  [SUCCESS] Parameter Discovery negotiated.\n";
        std::cout << "  [CURRENT DEMAND] Requesting: " << vehicleBms.getTargetRequestedCurrent() << " Amps from station.\n";
    }

    // 3. Serialize status onto the vehicle communication bus
    CanFdMessage powertrainFrame = vehicleBms.generatePowertrainCanFrame();
    std::cout << "\n[CAN_BUS] Broadcasting Frame 0x" << std::hex << powertrainFrame.arbitrationId 
              << " -> State Token: 0x" << static_cast<int>(powertrainFrame.payload[0])
              << " | Target Allocation: " << std::dec << static_cast<int>(powertrainFrame.payload[3]) << "A\n";

    std::cout << "\n========================================================\n";
    return 0;
}