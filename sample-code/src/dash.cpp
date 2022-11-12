#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>

#include "CFA10100_defines.h"

#include "EVE_base.h"
#include "EVE_draw.h"

#include "dash.h"

#include "teensy_can.h"
#include "virtualTimer.h"

TeensyCAN<1> can_bus{};

VirtualTimerGroup timer_group{};

CANSignal<uint32_t, 0, 32, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0), false> can_rx_test;

CANRXMessage<1> rx_message{can_bus, 0x100, can_rx_test};

uint8_t r;
uint8_t g;
uint8_t b;

void GetCAN() {
  can_bus.Tick();
}

void InitializeDash(void) {
  can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  can_bus.RegisterRXMessage(rx_message);  // Temporary workaround
  r = 0xff;
  g = 0x00;
  b = 0x80;

  timer_group.AddTimer(10, GetCAN);
}

uint16_t AddDashToDisplayList(uint16_t FWol) {
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(r, g, b));
  FWol = EVE_PrintF(
    FWol,
    LCD_WIDTH / 2,
    LCD_HEIGHT / 2,
    25, // font size
    EVE_OPT_CENTER,
    "%d",
    uint32_t(can_rx_test)
  );

  timer_group.Tick(millis());

  return(FWol);
}
