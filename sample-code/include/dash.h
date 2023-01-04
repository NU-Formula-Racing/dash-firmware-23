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
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_wheel_speed_signal;
  CANRXMessage<1> rx_ptrain{hp_can_bus, 0x420, motor_temp_signal};
  CANRXMessage<1> rx_flwheel{lp_can_bus, 0x400, fl_wheel_speed_signal};

  void GetCAN();
};

#endif
