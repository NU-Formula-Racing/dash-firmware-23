#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>
#include <math.h>

#include "CFA10100_defines.h"

#include "EVE_base.h"
#include "EVE_draw.h"

#include "dash.h"

#include "teensy_can.h"
#include "virtualTimer.h"

TeensyCAN<1> can_bus{};

VirtualTimerGroup timer_group{};

CANSignal<float, 0, 32, CANTemplateConvertFloat(0.01), CANTemplateConvertFloat(0), true> can_rx_test;

CANRXMessage<1> rx_message{can_bus, 0x100, can_rx_test};

double rgb[3] = {0, 0, 1};
uint16_t radius = 32;

void GetCAN() {
  can_bus.Tick();
  rgb[0] = can_rx_test;
  rgb[2] = 1.0 - can_rx_test;
  radius = 32 + uint16_t(32 * can_rx_test);
}

void InitializeDash(void) {
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  can_bus.RegisterRXMessage(rx_message);  // Temporary workaround

  timer_group.AddTimer(1, GetCAN);
}

uint16_t AddDashToDisplayList(uint16_t FWol) {
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
    uint8_t(rgb[0] * 255),
    uint8_t(rgb[1] * 255),
    uint8_t(rgb[2] * 255)
  ));
  // 800 x 480
  FWol = EVE_Point(
    FWol,
    LCD_WIDTH * 8,
    LCD_HEIGHT * 8,
    radius * 8
  );

  timer_group.Tick(millis());

  return(FWol);
}
