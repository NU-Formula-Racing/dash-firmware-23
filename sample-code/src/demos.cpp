#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>

#include "CFA10100_defines.h"

#include "EVE_base.h"
#include "EVE_draw.h"

#include "demos.h"

uint8_t r;
uint8_t g;
uint8_t b;

int screentest;
int timeoflastupdate;

void Initialize_Bounce_Demo(void) {
  Serial.println("includes work");

  r = 0xff;
  g = 0x00;
  b = 0x80;

  screentest = 0;
  timeoflastupdate = millis();
}

uint16_t Add_Bounce_To_Display_List(uint16_t FWol) {
  if (millis() - timeoflastupdate >= 1000) {
    timeoflastupdate = millis();
    screentest++;
  }

  FWol = EVE_Cmd_Dat_0(FWol, EVE_ENC_COLOR_RGB(r, g, b));
                     
  FWol = EVE_PrintF(
    FWol,
    LCD_WIDTH / 2, LCD_HEIGHT / 2,
    25,  //font
    EVE_OPT_CENTER,
    "%d",
    screentest
  );

  return(FWol);
}
