// I'll refactor this later...
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

double rgb[3] = {0, 0, 1};
uint16_t radius = 32;

void Dash::GetCAN() {
  can_bus1.Tick();
  can_bus2.Tick();
  rgb[0] = float_signal;
  rgb[2] = 1.0 - float_signal;
  radius = 32 + uint16_t(32 * float_signal);
}

void Dash::Initialize() {
  can_bus1.Initialize(ICAN::BaudRate::kBaud1M);
  can_bus2.Initialize(ICAN::BaudRate::kBaud1M);
  can_bus1.RegisterRXMessage(rx_message1);  // Temporary workaround
  can_bus2.RegisterRXMessage(rx_message2);

  // Light or Dark
  mode = 0;

  timer_group.AddTimer(1, [this]() { this->GetCAN(); });
  timer_group.AddTimer(3000, [this]() { mode = 1 - mode; });
}

uint16_t Dash::UpdateBackground(uint16_t FWol, uint8_t r, uint8_t g, uint8_t b) {
  return EVE_Cmd_Dat_0(FWol, EVE_ENC_CLEAR_COLOR_RGB(r, g, b));
}

uint16_t Dash::AddToDisplayList(uint16_t FWol) {
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
    uint8_t(rgb[0] * 255),
    uint8_t(rgb[1] * 255),
    uint8_t(rgb[2] * 255)
  ));

  // Draw a circle in the center of the screen 800 x 480
  // FWol = EVE_Point(
  //   FWol,
  //   LCD_WIDTH * 8,
  //   LCD_HEIGHT * 8,
  //   radius * 8
  // );
  FWol = EVE_Filled_Rectangle(
    FWol,
    20,
    (LCD_HEIGHT / 2) - 10,
    uint16_t(float_signal * (LCD_WIDTH - 50)) + 30,
    (LCD_HEIGHT / 2) + 10
  );

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
    uint8_t(mode * 255),
    uint8_t(mode * 255),
    uint8_t(mode * 255)
  ));
  FWol = EVE_PrintF(
    FWol,
    LCD_WIDTH / 2,
    (LCD_HEIGHT / 2) - 42,
    25,
    EVE_OPT_CENTER,
    "Test Text Color"
  );

  timer_group.Tick(millis());

  return(FWol);
}
