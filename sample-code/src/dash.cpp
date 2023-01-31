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

#include "mutable_array.h"

double rgb[3] = {0, 0, 1};
uint16_t radius = 32;



void Dash::GetCAN() {
  hp_can_bus.Tick();
  lp_can_bus.Tick();
  // rgb[0] = float_signal;
  // rgb[2] = 1.0 - float_signal;
  // radius = 32 + uint16_t(32 * float_signal);
}

void Dash::Initialize() {
  hp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  lp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  hp_can_bus.RegisterRXMessage(rx_ptrain);  // Temporary workaround
  lp_can_bus.RegisterRXMessage(rx_bmssoe);
  lp_can_bus.RegisterRXMessage(rx_bmsstat);
  lp_can_bus.RegisterRXMessage(rx_bmsstat);
  lp_can_bus.RegisterRXMessage(rx_flwheel);
  lp_can_bus.RegisterRXMessage(rx_frwheel);
  lp_can_bus.RegisterRXMessage(rx_blwheel);
  lp_can_bus.RegisterRXMessage(rx_brwheel);  

  // Light or Dark
  mode = 0;
  index = 0;
  arr.AddString("Error 1");
  arr.AddString("Error 2");
  arr.AddString("Error 3");


  timer_group.AddTimer(1, [this]() { this->GetCAN(); });
  timer_group.AddTimer(3000, [this]() { mode = 1 - mode; });
  timer_group.AddTimer(1000, [this]() { if(index >= arr.Length()-1) {index = 0;}
  else{index = index + 1;}});
  timer_group.AddTimer(6000, [this]() {arr.Remove(2);});
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

  const float kWheelSpeed = 80.0;
  const float kMotorTemp = 10.0;
  float fl_wheel_speed = static_cast<float>(fl_wheel_speed_signal);
  float motor_temp = static_cast<float>(motor_temp_signal);

 


  // Draw a circle in the center of the screen 800 x 480
  // FWol = EVE_Point(
  //   FWol,
  //   LCD_WIDTH * 8,
  //   LCD_HEIGHT * 8,
  //   radius * 8
  // );

  

  error_count = 2;

  FWol = EVE_Filled_Rectangle(
    FWol,
    20,
    (LCD_HEIGHT / 2) - 10,
    uint16_t(0 * (LCD_WIDTH - 50)) + 30,
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
    (LCD_HEIGHT / 2) - 84,
    25,
    EVE_OPT_CENTER,
    "Errors: %s",
    arr[index]
  );

  FWol = EVE_PrintF(
    FWol,
    LCD_WIDTH / 2,
    (LCD_HEIGHT / 2) - 42,
    25,
    EVE_OPT_CENTER,
    "Motor temp signal: %f",
    motor_temp
  );
  FWol = EVE_PrintF(
    FWol,
    LCD_WIDTH / 2,
    (LCD_HEIGHT / 2),
    25,
    EVE_OPT_CENTER,
    "Wheel speed signal: %f",
    fl_wheel_speed
  );

  // Serial.print("Motor temp: ");
  // Serial.print(motor_temp_signal);
  // Serial.print("\n");
  // Serial.print("FL wheel speed: ");
  // Serial.print(fl_wheel_speed_signal);

  Serial.print("\n");

  timer_group.Tick(millis());

  return(FWol);
}
