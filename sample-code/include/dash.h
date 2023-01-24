#pragma once
#include <stdint.h>
#include "teensy_can.h"
#include "virtualTimer.h"

#ifndef __DASH_H__
#define __DASH_H__

// Will Refactor later
class Dash {
public:
  char mode;

  void Initialize();
  uint16_t UpdateBackground(uint16_t FWol, uint8_t r, uint8_t g, uint8_t b);
  uint16_t AddToDisplayList(uint16_t FWol);
private:
  TeensyCAN<2> hp_can_bus{};
  TeensyCAN<1> lp_can_bus{};
  VirtualTimerGroup timer_group{};
  CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> motor_temp_signal;
  CANSignal<float, 32, 8, CANTemplateConvertFloat(0.5), CANTemplateConvertFloat(0), false> bms_soc_signal;
  CANSignal<float, 24, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), false> batt_voltage_signal;
  CANSignal<float, 48, 16, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), true> batt_current_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fr_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> bl_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> br_wheel_speed_signal;
  CANRXMessage<1> rx_ptrain{hp_can_bus, 0x420, motor_temp_signal};
  CANRXMessage<1> rx_bmssoe{lp_can_bus, 0x576, batt_voltage_signal, batt_current_signal};
  CANRXMessage<1> rx_bmsstat{lp_can_bus, 0x241, bms_soc_signal};
  CANRXMessage<1> rx_flwheel{lp_can_bus, 0x400, fl_wheel_speed_signal};
  CANRXMessage<1> rx_frwheel{lp_can_bus, 0x401, fr_wheel_speed_signal};
  CANRXMessage<1> rx_blwheel{lp_can_bus, 0x402, bl_wheel_speed_signal};
  CANRXMessage<1> rx_brwheel{lp_can_bus, 0x403, br_wheel_speed_signal};
 
  void GetCAN();
};

#endif
