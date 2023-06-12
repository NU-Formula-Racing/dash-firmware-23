#pragma once
#include <stdint.h>
#include "teensy_can.h"
#include "virtualTimer.h"
#include "mutable_array.h"
#include "inverter_driver.h"

#ifndef __DASH_H__
#define __DASH_H__
// #define DEBUG_MODE

// Will Refactor later
class Dash
{
public:
    char mode;
    uint16_t index;

    int error_banner;

    MutableStringArray arr;

    MutableStringArray *arr_ref = &arr;

    void Initialize();
    uint16_t UpdateBackground(uint16_t FWol, uint8_t r, uint8_t g, uint8_t b);
    uint16_t AddToDisplayList(uint16_t FWol);
    float WheelSpeedAvg();
    float wheel_speed_avg;
    float BrakeTempAvg();
    float brake_temp_avg;
    uint8_t *BarColorPicker(uint16_t bad, uint16_t ok, float act_val, bool is_descending);

    void GetCAN();
    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> imd_state_signal;

private:
    TeensyCAN<1> hp_can_bus{};
    TeensyCAN<2> lp_can_bus{};
    VirtualTimerGroup timer_group{};
    Inverter inverter{hp_can_bus};
    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> motor_temp_signal;
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(-40), false> coolant_temp_signal;
    CANSignal<float, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> max_cell_temp_signal;
    CANSignal<float, 40, 8, CANTemplateConvertFloat(0.5), CANTemplateConvertFloat(0), false> bms_soc_signal;
    CANSignal<float, 24, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> batt_voltage_signal;
    CANSignal<float, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> batt_temp_signal;
    CANSignal<float, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), true> accel_percent_signal;
    CANSignal<float, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> brake_percent_signal;
    CANSignal<float, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> torque_limit_signal;
    CANSignal<float, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> torque_request_signal;
    CANSignal<float, 48, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), true> batt_current_signal;
    CANSignal<float, 0, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> fault_summary_signal;
    CANSignal<float, 1, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> undervoltage_fault_signal;
    CANSignal<float, 2, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overvoltage_fault_signal;
    CANSignal<float, 3, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> undertemp_fault_signal;
    CANSignal<float, 4, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overtemp_fault_signal;
    CANSignal<float, 5, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> overcurrent_fault_signal;
    CANSignal<float, 6, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> external_kill_fault_signal;
    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_wheel_speed_signal;
    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fr_wheel_speed_signal;
    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> bl_wheel_speed_signal;
    CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> br_wheel_speed_signal;
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_brake_temperature;
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fr_brake_temperature;
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> bl_brake_temperature;
    CANSignal<float, 16, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> br_brake_temperature;
    CANSignal<float, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> tractive_system_status_signal;
    CANSignal<bool, 8, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> release_accel_signal;
    CANSignal<bool, 9, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> potentiometer_signal;
    CANSignal<bool, 10, 1, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> gnd_3v3_signal;
    CANSignal<uint32_t, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> rtc_signal;

    CANRXMessage<2> rx_ptrain{hp_can_bus, 0x420, motor_temp_signal, coolant_temp_signal};
    CANRXMessage<3> rx_bmssoe{hp_can_bus, 0x240, batt_voltage_signal, batt_current_signal, batt_temp_signal};
    CANRXMessage<4> rx_throttlevals{hp_can_bus, 0x300, accel_percent_signal, brake_percent_signal, torque_limit_signal, torque_request_signal};
    CANRXMessage<1> rx_bmsstat{hp_can_bus, 0x241, bms_soc_signal};
    CANRXMessage<7> rx_bmsfaults{lp_can_bus, 0x250, fault_summary_signal, undervoltage_fault_signal, overvoltage_fault_signal, undertemp_fault_signal, overtemp_fault_signal, overcurrent_fault_signal, external_kill_fault_signal};
    CANRXMessage<4> rx_throttlestat{hp_can_bus, 0x301, tractive_system_status_signal, release_accel_signal, potentiometer_signal, gnd_3v3_signal};
    CANRXMessage<2> rx_flwheel{lp_can_bus, 0x400, [this]()
                               { WheelSpeedAvg(); },
                               fl_wheel_speed_signal, fl_brake_temperature};
    CANRXMessage<2> rx_frwheel{lp_can_bus, 0x401, fr_wheel_speed_signal, fr_brake_temperature};
    CANRXMessage<2> rx_blwheel{lp_can_bus, 0x402, [this]()
                               { BrakeTempAvg(); },
                               bl_wheel_speed_signal, bl_brake_temperature};
    CANRXMessage<2> rx_brwheel{lp_can_bus, 0x403, br_wheel_speed_signal, br_brake_temperature};
    CANRXMessage<1> rx_imd_state{lp_can_bus, 0x600, imd_state_signal};
    CANRXMessage<1> rx_rtc{lp_can_bus, 0x440, rtc_signal};
    CANTXMessage<2> tx_throttlevals{lp_can_bus, 0x300, 2, 10, timer_group, accel_percent_signal, brake_percent_signal};
};

#endif
