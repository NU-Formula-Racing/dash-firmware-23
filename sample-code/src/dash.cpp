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
// #define RPM

double rgb[3] = {0, 0, 1};
uint16_t radius = 32;

void Dash::GetCAN()
{
  hp_can_bus.Tick();
  lp_can_bus.Tick();
}
float Dash::BrakeTempAvg()
{
  return brake_temp_avg = float((fl_brake_temperature + fr_brake_temperature + bl_brake_temperature + br_brake_temperature)) / 4.0;
  // Serial.print(fr_brake_temperature);
}

float Dash::WheelSpeedAvg()
{
  return wheel_speed_avg = float((fl_wheel_speed_signal + fr_wheel_speed_signal + bl_wheel_speed_signal + br_wheel_speed_signal)) / 4.0;
  // Serial.print(br_wheel_speed_signal);
}
void Dash::Initialize()
{
  hp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  lp_can_bus.Initialize(ICAN::BaudRate::kBaud1M);
  hp_can_bus.RegisterRXMessage(rx_ptrain); // Temporary workaround
  hp_can_bus.RegisterRXMessage(rx_bmssoe);
  // hp_can_bus.RegisterRXMessage(rx_bmssoe);
  hp_can_bus.RegisterRXMessage(rx_bmsstat);
  hp_can_bus.RegisterRXMessage(rx_bmsfaults);
  hp_can_bus.RegisterRXMessage(rx_throttlestat);
  hp_can_bus.RegisterRXMessage(rx_throttlevals);
  lp_can_bus.RegisterRXMessage(rx_flwheel);
  lp_can_bus.RegisterRXMessage(rx_frwheel);
  lp_can_bus.RegisterRXMessage(rx_blwheel);
  lp_can_bus.RegisterRXMessage(rx_brwheel);
  lp_can_bus.RegisterRXMessage(rx_imd_state);

  inverter.RequestRPM(10);

  // Light or Dark
  mode = 0;
  index = 0;
  arr.AddString("No Errors");
  timer_group.AddTimer(1, [this]()
                       { this->GetCAN(); });
  // timer_group.AddTimer(3000, [this]() { mode = 1 - mode; });
  timer_group.AddTimer(1000, [this]()
                       { if(index >= arr.Length()-1) {index = 0;}
  else{index = index + 1;} });
}

uint16_t Dash::UpdateBackground(uint16_t FWol, uint8_t r, uint8_t g, uint8_t b)
{
  return EVE_Cmd_Dat_0(FWol, EVE_ENC_CLEAR_COLOR_RGB(r, g, b));
}

uint8_t *Dash::BarColorPicker(uint16_t bad, uint16_t ok, float act_val, bool is_descending)
{
  uint8_t *bar_rgb = new uint8_t[3];

  if (is_descending)
  {
    if (act_val > bad)
    {
      bar_rgb[0] = 255;
      bar_rgb[1] = 0;
      bar_rgb[2] = 0;
    }
    else if (act_val > ok)
    {
      bar_rgb[0] = 255;
      bar_rgb[1] = 255;
      bar_rgb[2] = 0;
    }
    else
    {
      bar_rgb[0] = 34;
      bar_rgb[1] = 139;
      bar_rgb[2] = 34;
    }
  }
  else
  {
    if (act_val < bad)
    {
      bar_rgb[0] = 255;
      bar_rgb[1] = 0;
      bar_rgb[2] = 0;
    }
    else if (act_val < ok)
    {
      bar_rgb[0] = 255;
      bar_rgb[1] = 255;
      bar_rgb[2] = 0;
    }
    else
    {
      bar_rgb[0] = 34;
      bar_rgb[1] = 139;
      bar_rgb[2] = 34;
    }
  }

  return bar_rgb;
}

uint16_t Dash::AddToDisplayList(uint16_t FWol)
{
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(rgb[0] * 255),
                                 uint8_t(rgb[1] * 255),
                                 uint8_t(rgb[2] * 255)));

  float coolant_temp = static_cast<float>(coolant_temp_signal);
  float batt_charge = static_cast<float>(bms_soc_signal);
  float max_cell_temp = static_cast<float>(max_cell_temp_signal);

  // max is 600
  float batt_voltage = static_cast<float>(batt_voltage_signal);
  float batt_temp = static_cast<float>(batt_temp_signal);
  float batt_current = static_cast<float>(batt_current_signal);
  // Serial.print(batt_voltage);

  float fault_summary = static_cast<float>(fault_summary_signal);
  float undervoltage_fault = static_cast<float>(undervoltage_fault_signal);
  float overvoltage_fault = static_cast<float>(overvoltage_fault_signal);
  float undertemp_fault = static_cast<float>(undertemp_fault_signal);
  float overtemp_fault = static_cast<float>(overtemp_fault_signal);
  float overcurrent_fault = static_cast<float>(overcurrent_fault_signal);
  float external_kill = static_cast<float>(external_kill_fault_signal);

  float accel_percent = static_cast<float>(accel_percent_signal);
  float torque_limit = static_cast<float>(torque_limit_signal);
  // int tractive_system = static_cast<float>(tractive_system_status_signal);

  char tractive_system_status = 'O';

  if (tractive_system_status_signal == 0)
  {
    tractive_system_status = 'O';
  }
  else if (tractive_system_status_signal == 1)
  {
    tractive_system_status = 'N';
  }
  else if (tractive_system_status_signal == 2)
  {
    tractive_system_status = 'D';
  }
  else
  {
    tractive_system_status = 'F';
  }

  if (undervoltage_fault == 1 && !(arr.Contains("Undervoltage Fault")))
  {
    arr.AddString("Undervoltage Fault");
  }
  else
  {
    if (undervoltage_fault == 0 && arr.Contains("Undervoltage Fault"))
    {
      arr.Remove("Undervoltage Fault");
    }
  }

  if (overvoltage_fault == 1 && !(arr.Contains("Overvoltage Fault")))
  {
    arr.AddString("Overvoltage Fault");
  }
  else
  {
    if (overvoltage_fault == 0 && arr.Contains("Overvoltage Fault"))
    {
      arr.Remove("Overvoltage Fault");
    }
  }

  if (undertemp_fault == 1 && !(arr.Contains("Undertemp Fault")))
  {
    arr.AddString("Undertemp Fault");
  }
  else
  {
    if (undertemp_fault == 0 && arr.Contains("Undertemp Fault"))
    {
      arr.Remove("Undertemp Fault");
    }
  }

  if (overtemp_fault == 1 && !(arr.Contains("Overtemp Fault")))
  {
    arr.AddString("Overtemp Fault");
  }
  else
  {
    if (overtemp_fault == 0 && arr.Contains("Overtemp Fault"))
    {
      arr.Remove("Overtemp Fault");
    }
  }

  if (overcurrent_fault == 1 && !(arr.Contains("Overcurrent Fault")))
  {
    arr.AddString("Overcurrent Fault");
  }
  else
  {
    if (overcurrent_fault == 0 && arr.Contains("Overcurrent Fault"))
    {
      arr.Remove("Overcurrent Fault");
    }
  }

  if (external_kill == 1 && !(arr.Contains("External Kill Fault")))
  {
    arr.AddString("External Kill Fault");
  }
  else
  {
    if (external_kill == 0 && (arr.Contains("External Kill Fault")))
    {
      arr.Remove("External Kill Fault");
    }
  }

  if (arr.Contains("No Errors") && arr.Length() > 1)
  {
    arr.Remove("No Errors");
    error_banner = 1;
  }
  else if (!arr.Contains("No Errors") && arr.Length() == 0)
  {
    arr.AddString("No Errors");
    error_banner = 0;
  }

  int batt_height = (int)batt_charge;
  float motor_temp = static_cast<float>(motor_temp_signal);

  // Serial.print(batt_height);
  // Serial.print("\n");
  // Serial.print(motor_temp);
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
                                 uint8_t(mode * 255)));

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 126,
      30,
      EVE_OPT_CENTER,
      "Display Mode: Debug");

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 84,
      24.3,
      EVE_OPT_CENTER,
      "Errors: %s",
      arr[index]);

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 42,
      24.3,
      EVE_OPT_CENTER,
      "Motor temp signal: %.2f C",
      motor_temp);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2),
      24.3,
      EVE_OPT_CENTER,
      "Wheel speed signal: %.2f MPH",
      wheel_speed_avg);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 42,
      24.3,
      EVE_OPT_CENTER,
      "Battery voltage signal: %.2f V",
      batt_voltage);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 84,
      24.3,
      EVE_OPT_CENTER,
      "Battery current signal: %.2f A",
      batt_current);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 126,
      24.3,
      EVE_OPT_CENTER,
      "Battery charge: %.2f%%",
      batt_charge);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 126,
      24.3,
      EVE_OPT_CENTER,
      "Front Brake Temp: %.2f%%",
      front_brake_temp_avg);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) + 126,
      24.3,
      EVE_OPT_CENTER,
      "Back Brake Temp: %.2f%%",
      back_brake_temp_avg);
#endif

#ifndef DEBUG_MODE
  if (error_banner == 1)
  {
    // FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
    //                                uint8_t(208),
    //                                uint8_t(52),
    //                                uint8_t(44)));

    // FWol = EVE_Filled_Rectangle(
    //     FWol,
    //     275,
    //     LCD_HEIGHT - 425,
    //     uint16_t(0 * (LCD_WIDTH - 50)) + 525,
    //     LCD_HEIGHT - 350);

    FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                   uint8_t(0),
                                   uint8_t(0),
                                   uint8_t(0)));
    FWol = EVE_PrintF(
        FWol,
        LCD_WIDTH / 2,
        (LCD_HEIGHT - 387),
        30,
        EVE_OPT_CENTER,
        "ERROR");
  }

  batt_charge=100;
  uint8_t *bar_rgb2 = BarColorPicker(10, 50, batt_charge, false);
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(bar_rgb2[0]),
                                 uint8_t(bar_rgb2[1]),
                                 uint8_t(bar_rgb2[2])));
  batt_voltage = 100;
  FWol = EVE_Filled_Rectangle(
      FWol,
      50,
      LCD_HEIGHT - 60,
      uint16_t(0 * (LCD_WIDTH - 50)) + 100,
      LCD_HEIGHT - 50 - (batt_charge*2.5));


  uint8_t *bar_rgb3 = BarColorPicker(350, 500, batt_voltage, false);
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(bar_rgb3[0]),
                                 uint8_t(bar_rgb3[1]),
                                 uint8_t(bar_rgb3[2])));
  FWol = EVE_Filled_Rectangle(
      FWol,
      150,
      LCD_HEIGHT - 60,
      uint16_t(0 * (LCD_WIDTH - 50)) + 200,
      LCD_HEIGHT - 50 - (batt_voltage / 6) * 2.5);

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(0),
                                 uint8_t(0),
                                 uint8_t(0)));

  // batt_temp = 100;
  uint8_t *bar_rgb4 = BarColorPicker(60, 50, batt_temp, true);
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(bar_rgb4[0]),
                                 uint8_t(bar_rgb4[1]),
                                 uint8_t(bar_rgb4[2])));
  //   Serial.print(bar_rgb[0]);
  //   Serial.print(bar_rgb[1]);
  //   Serial.print(bar_rgb[2]);
  FWol = EVE_Filled_Rectangle(
      FWol,
      725,
      LCD_HEIGHT - 60,
      uint16_t(0 * (LCD_WIDTH - 50)) + 775,
      LCD_HEIGHT - 50 - (batt_temp / 6) * 3);

  uint8_t *bar_rgb5 = BarColorPicker(60, 40, coolant_temp, true);
  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(bar_rgb5[0]),
                                 uint8_t(bar_rgb5[1]),
                                 uint8_t(bar_rgb5[2])));
  FWol = EVE_Filled_Rectangle(
      FWol,
      655,
      LCD_HEIGHT - 60,
      uint16_t(0 * (LCD_WIDTH - 50)) + 705,
      LCD_HEIGHT - 50 - (coolant_temp / 2) * 3);

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(0),
                                 uint8_t(0),
                                 uint8_t(0)));
  FWol = EVE_Filled_Rectangle(
      FWol,
      585,
      LCD_HEIGHT - 60,
      uint16_t(0 * (LCD_WIDTH - 50)) + 635,
      LCD_HEIGHT - 50 - (brake_temp_avg / 6) * 3);

  FWol = EVE_Filled_Rectangle(
      FWol,
      35,
      LCD_HEIGHT - 375,
      765,
      LCD_HEIGHT - 425);

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(255),
                                 uint8_t(255),
                                 uint8_t(255)));

  FWol = EVE_Filled_Rectangle(
      FWol,
      40,
      LCD_HEIGHT - 380,
      760,
      LCD_HEIGHT - 420);

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(147),
                                 uint8_t(112),
                                 uint8_t(219)));

  FWol = EVE_Filled_Rectangle(
      FWol,
      40,
      LCD_HEIGHT - 380,
      uint16_t(40 + (((accel_percent / 100) * (torque_limit / 100))) * 720),
      LCD_HEIGHT - 420);

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2 + 65,
      (LCD_HEIGHT - 350),
      23,
      EVE_OPT_CENTER,
      "Torque: %.2f Nm",
      accel_percent*torque_limit*230);

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
                                 uint8_t(0),
                                 uint8_t(0),
                                 uint8_t(0)));

  FWol = EVE_Filled_Rectangle(
      FWol,
      uint16_t(40 + ((torque_limit) / 100) * 720) + 1,
      LCD_HEIGHT - 380,
      uint16_t(40 + ((torque_limit) / 100) * 720) + 3,
      LCD_HEIGHT - 420);

  // FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(
  //   uint8_t(mode * 255),
  //   uint8_t(mode * 255),
  //   uint8_t(mode * 255)
  // ));

  // FWol = EVE_PrintF(
  //   FWol,
  //   LCD_WIDTH / 2,
  //   (LCD_HEIGHT / 2) - 126,
  //   30,
  //   EVE_OPT_CENTER,
  //   "Display Mode: Non-Debug"
  // );

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2) - 84,
      24.3,
      EVE_OPT_CENTER,
      "Errors: %s",
      arr[index]);

  FWol = EVE_RomFont(
      FWol,
      1,
      34);

#ifndef RPM
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2),
      1,
      EVE_OPT_CENTER,
      "%.0f",
      wheel_speed_avg);
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2 + 65,
      (LCD_HEIGHT / 2 + 15),
      29,
      EVE_OPT_CENTER,
      "mph");
#endif

#ifdef RPM
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT / 2),
      1,
      EVE_OPT_CENTER,
      "%.0f",
      inverter.GetRPM());
  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2 + 65,
      (LCD_HEIGHT / 2 + 15),
      29,
      EVE_OPT_CENTER,
      "rpm");
#endif

  FWol = EVE_PrintF(
      FWol,
      80,
      (LCD_HEIGHT - 48),
      23,
      EVE_OPT_CENTER,
      "Batt %%");
  
  FWol = EVE_PrintF(
      FWol,
      80,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "%.2f",
      batt_charge);
  
  FWol = EVE_PrintF(
      FWol,
      175,
      (LCD_HEIGHT - 48),
      23,
      EVE_OPT_CENTER,
      "Batt V");

  FWol = EVE_PrintF(
      FWol,
      175,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "%.2f",
      batt_voltage);

  FWol = EVE_PrintF(
      FWol,
      LCD_WIDTH / 2,
      (LCD_HEIGHT - 78),
      1,
      EVE_OPT_CENTER,
      "%c",
      tractive_system_status);

  FWol = EVE_PrintF(
      FWol,
      750,
      (LCD_HEIGHT - 48),
      23,
      EVE_OPT_CENTER,
      "Batt T");

  FWol = EVE_PrintF(
      FWol,
      750,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "%.2f",
      batt_temp);


  FWol = EVE_PrintF(
      FWol,
      680,
      (LCD_HEIGHT - 48),
      23,
      EVE_OPT_CENTER,
      "Cool T");

  FWol = EVE_PrintF(
      FWol,
      680,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "%.2f",
      coolant_temp);

  FWol = EVE_PrintF(
      FWol,
      610,
      (LCD_HEIGHT - 48),
      23,
      EVE_OPT_CENTER,
      "Brake T");

  FWol = EVE_PrintF(
      FWol,
      610,
      (LCD_HEIGHT - 38),
      23,
      EVE_OPT_CENTER,
      "%.2f",
      brake_temp_avg);

#endif

  // Serial.printf("time since fl last receive: %d \n", rx_flwheel.GetTimeSinceLastReceive());
  // Serial.printf("time since fr last receive: %d \n", rx_frwheel.GetTimeSinceLastReceive());
  // Serial.printf("time since bl last receive: %d \n", rx_blwheel.GetTimeSinceLastReceive());
  // Serial.printf("time since br last receive: %d \n", rx_brwheel.GetTimeSinceLastReceive());

  timer_group.Tick(millis());

  return (FWol);
}
