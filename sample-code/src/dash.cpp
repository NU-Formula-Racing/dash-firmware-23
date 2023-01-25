// I'll refactor this later...
#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#include "CFA10100_defines.h"

#include "EVE_base.h"
#include "EVE_draw.h"

#include "dash.h"

#include "teensy_can.h"
#include "virtualTimer.h"


double rgb[3] = {0, 0, 1};
uint16_t radius = 32;

void Dash::GetCAN() {
  hp_can_bus.Tick();
  lp_can_bus.Tick();
  // rgb[0] = float_signal;
  // rgb[2] = 1.0 - float_signal;
  // radius = 32 + uint16_t(32 * float_signal);
}

void Dash::ErrorListAdd() {

}

void Dash::ErrorListRemove() {

}

void Dash::UpdateErrorList() {
  Node* one = new Node();
  Node* two = new Node();

  one->msg = "Error 1";
  size = size + 1;
  two->msg = "Error 2";
  size = size + 2;

  one->next = two;
  two->next = NULL;

  head = one;

  // while (head != NULL) {
  //   head = head->next;
  // }
}

void Dash::Initialize() {
  hp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  lp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  hp_can_bus.RegisterRXMessage(rx_ptrain);  // Temporary workaround
  lp_can_bus.RegisterRXMessage(rx_flwheel);
  lp_can_bus.RegisterRXMessage(rx_frwheel);
  lp_can_bus.RegisterRXMessage(rx_blwheel);
  lp_can_bus.RegisterRXMessage(rx_brwheel);

  // Light or Dark
  mode = 0;
  size = 0;
  counter = 0;

  timer_group.AddTimer(1, [this]() { this->GetCAN(); });
  timer_group.AddTimer(3000, [this]() { mode = 1 - mode; });
  timer_group.AddTimer(1000, [this]() { this->UpdateErrorList(); });

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
  float motor_temp = static_cast<float>(motor_temp_signal);
  float wheel_speed_avg = static_cast<float>((fl_wheel_speed + fr_wheel_speed + bl_wheel_speed + br_wheel_speed) / 4.0);

  // Draw a circle in the center of the screen 800 x 480
  // FWol = EVE_Point(
  //   FWol,
  //   LCD_WIDTH * 8,
  //   LCD_HEIGHT * 8,
  //   radius * 8
  // );
  // Draw a blue rectangle on the left side of the screen
  // FWol = EVE_Filled_Rectangle(
  //   FWol,
  //   20,
  //   (LCD_HEIGHT / 2) - 10,
  //   uint16_t(0 * (LCD_WIDTH - 50)) + 30,
  //   (LCD_HEIGHT / 2) + 10
  // );

  // String s = "this is a longer string";
  // const char* str = msg[i].c_str();
  
  // Node* head;

  // Node* one = new Node();
  // Node* two = new Node();

  // one->msg = "Error 1";
  // size = size + 1;
  // two->msg = "Error 2";
  // size = size + 2;

  // one->next = two;
  // two->next = NULL;

  // head = one;

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
    "Error Message: %s",
    head->msg
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
    wheel_speed_avg
  );

  Serial.print("Motor temp: ");
  Serial.print(motor_temp_signal);
  Serial.print("\n");
  Serial.print("Avg wheel speed: ");
  Serial.print(wheel_speed_avg);
  Serial.print("\n");
  Serial.print("Error Message: ");
  //Serial.print();
  Serial.print("\n");

  timer_group.Tick(millis());

  return(FWol);
}
