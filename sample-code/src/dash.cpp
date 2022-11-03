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

// Receive a uint8_t over CAN
const uint8_t kPdrStart = 0;
const int kPdrFactor = 0;
const int kPdrOffset = 0;
const int kPdrMsgID = 0x100;
const ICAN::BaudRate kPdrBaud = ICAN::BaudRate::kBaud1M;

CANSignal<
  uint8_t,
  kPdrStart,
  8,
  CANTemplateConvertFloat(kPdrFactor),
  CANTemplateConvertFloat(kPdrOffset),
  false
> can_rx_test;

CANRXMessage<1> rx_message{can_bus, kPdrMsgID, can_rx_test};

uint8_t r = 0xff;
uint8_t g = 0x00;
uint8_t b = 0x80;
uint8_t display_value = 0;

void GetCAN() {
  display_value = can_rx_test;
  can_bus.Tick();
}

void InitializeDash(void) {
  can_bus.Initialize(kPdrBaud);
  timer_group.AddTimer(1000, GetCAN);
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
    display_value
  );

  return(FWol);
}
