#pragma once
#include <stdint.h>
#include "teensy_can.h"
#include "virtualTimer.h"
#include <bits/stdc++.h>
#include <iostream>

#ifndef __DASH_H__
#define __DASH_H__

// Will Refactor later
class Dash {
public:
  char mode;
  int size;
  int counter;

  void Initialize();

  class Node {
      public:
    char* msg;
    Node* next;
  };

  Node* head;
  

  uint16_t UpdateBackground(uint16_t FWol, uint8_t r, uint8_t g, uint8_t b);
  uint16_t AddToDisplayList(uint16_t FWol);
private:
  TeensyCAN<2> hp_can_bus{};
  TeensyCAN<1> lp_can_bus{};
  VirtualTimerGroup timer_group{};
  CANSignal<float, 0, 16, CANTemplateConvertFloat(1), CANTemplateConvertFloat(-40), false> motor_temp_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fl_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> fr_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> bl_wheel_speed_signal;
  CANSignal<float, 0, 16, CANTemplateConvertFloat(0.1), CANTemplateConvertFloat(0), false> br_wheel_speed_signal;
  
  CANRXMessage<1> rx_flwheel{lp_can_bus, 0x400, fl_wheel_speed_signal};
  CANRXMessage<1> rx_frwheel{lp_can_bus, 0x401, fr_wheel_speed_signal};
  CANRXMessage<1> rx_blwheel{lp_can_bus, 0x402, bl_wheel_speed_signal};
  CANRXMessage<1> rx_brwheel{lp_can_bus, 0x403, br_wheel_speed_signal};
  CANRXMessage<1> rx_ptrain{hp_can_bus, 0x420, motor_temp_signal};
  void GetCAN();
  void UpdateErrorList();
  void ErrorListAdd();
  void ErrorListRemove();
};

#endif
