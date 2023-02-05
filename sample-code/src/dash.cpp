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

// #define DEBUG_MODE

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
  hp_can_bus.RegisterRXMessage(rx_bmssoe);
  //hp_can_bus.RegisterRXMessage(rx_bmssoe);
  hp_can_bus.RegisterRXMessage(rx_bmsstat);
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


  timer_group.AddTimer(10, [this]() { this->GetCAN(); });
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


  // const float kWheelSpeed = 80.0;
  // const float kMotorTemp = 10.0;
  float fl_wheel_speed = static_cast<float>(fl_wheel_speed_signal);
  float fr_wheel_speed = static_cast<float>(fr_wheel_speed_signal);
  float bl_wheel_speed = static_cast<float>(bl_wheel_speed_signal);
  float br_wheel_speed = static_cast<float>(br_wheel_speed_signal);
  float wheel_speed_avg = static_cast<float>((fl_wheel_speed + fr_wheel_speed + bl_wheel_speed + br_wheel_speed) / 4.0);
  
  float motor_temp = static_cast<float>(motor_temp_signal);

  float batt_voltage = static_cast<float>(batt_voltage_signal);

  float batt_current = static_cast<float>(batt_current_signal);

  float batt_charge = static_cast<float>(bms_soc_signal);

  int batt_height = (int) wheel_speed_avg;

  // Draw a circle in the center of the screen 800 x 480
  // FWol = EVE_Point(
  //   FWol,
  //   LCD_WIDTH * 8,
  //   LCD_HEIGHT * 8,
  //   radius * 8
  // );

  #ifdef DEBUG_MODE
    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
      uint8_t(mode * 255),
      uint8_t(mode * 255),
      uint8_t(mode * 255)
    ));

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 126,
      30,
      EVE_OPT_CENTER,
      "Display Mode: Debug"
    );

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 84,
      24.3,
      EVE_OPT_CENTER,
      "Errors: %s",
      arr[index]
    );

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 42,
      24.3,
      EVE_OPT_CENTER,
      "Motor temp signal: %.2f C",
      motor_temp
    );
    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2),
      24.3,
      EVE_OPT_CENTER,
      "Wheel speed signal: %.2f MPH",
      wheel_speed_avg
    );
    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 42,
      24.3,
      EVE_OPT_CENTER,
      "Battery voltage signal: %.2f V",
      batt_voltage
    );
    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 84,
      24.3,
      EVE_OPT_CENTER,
      "Battery current signal: %.2f A",
      batt_current
    );
    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 126,
      24.3,
      EVE_OPT_CENTER,
      "Battery charge: %.2f%%",
      batt_charge
    );
  #endif

  #ifndef DEBUG_MODE
    FWol = EVE_Filled_Rectangle(
    FWol,
    70,
    LCD_HEIGHT - 50,
    uint16_t(0 * (LCD_WIDTH - 50)) + 100,
    LCD_HEIGHT-50-(70-batt_height)*20);

    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
      uint8_t(mode * 255),
      uint8_t(mode * 255),
      uint8_t(mode * 255)
    ));

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 126,
      30,
      EVE_OPT_CENTER,
      "Display Mode: Non-Debug"
    );

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 84,
      24.3,
      EVE_OPT_CENTER,
      "Errors: %s",
      arr[index]
    );

    FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2),
      31.5,
      EVE_OPT_CENTER,
      "%.2f MPH",
      wheel_speed_avg
    );

    FWol = EVE_PrintF(
      FWol,
      85,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "Batt %%"
    );



  #endif


  // Serial.print("Motor temp: ");
  // Serial.print(motor_temp_signal);
  // Serial.print("\n");
  // Serial.print("FL wheel speed: ");
  // Serial.print(fl_wheel_speed_signal);

  Serial.print("\n");

  timer_group.Tick(millis());

  return(FWol);
}
