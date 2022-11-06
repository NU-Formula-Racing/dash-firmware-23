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

uint8_t r;
uint8_t g;
uint8_t b;
uint8_t display_value;

void GetCAN() {
  // display_value = can_rx_test;
  // can_bus.Tick();
  display_value++;
}
VirtualTimer can_timer(1000U, GetCAN, VirtualTimer::Type::kRepeating);

void InitializeDash(void) {
  can_bus.Initialize(kPdrBaud);
  r = 0xff;
  g = 0x00;
  b = 0x80;
  display_value = 0;
  can_timer.Start(millis());
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

  can_timer.Tick(millis());

  return(FWol);
}
