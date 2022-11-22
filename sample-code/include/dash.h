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
  TeensyCAN<1> can_bus1{};
  TeensyCAN<2> can_bus2{};
  VirtualTimerGroup timer_group{};
  CANSignal<float, 0, 32, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), true> float_signal;
  CANSignal<uint32_t, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> uint32_t_signal;
  CANRXMessage<1> rx_message1{can_bus1, 0x100, float_signal};
  CANRXMessage<1> rx_message2{can_bus2, 0x110, uint32_t_signal};

  void GetCAN();
};

#endif
