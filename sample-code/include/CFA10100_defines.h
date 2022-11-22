#ifndef __CFA10100_DEFINES_H__
#define __CFA10100_DEFINES_H__

#define EVE_DEVICE           (BT817)

#include "EVE_defines.h"

//============================================================================
//
// You can set DEBUG_LEVEL to:
//   DEBUG_NONE   (0K flash, serial not used)
//   DEBUG_STATUS (1.4K ~ 2.5K flash)
//   DEBUG_GEEK   (6.4K ~ 7K flash)
// to send messages to the console (Ctrl+Shift+M) in the arduino IDE.
#define DEBUG_LEVEL (DEBUG_GEEK)

//============================================================================
// Wiring for Seeeduino v4.2 (3.3v) connected to CFA10098 breakout for testing.
//   ARD      | Port | 10098/EVE           | Color
// -----------+------+---------------------|--------
//  #3/D3     |  PD3 | DEBUG_LED           | Green W/W
//  #7/D7     |  PD7 | EVE_INT             | Purple
//  #8/D8     |  PB0 | EVE_PD_NOT          | Blue
//  #9/D9     |  PB1 | EVE_CS_NOT          | Brown
// #10/D10    |  PB2 | SD_CS_NOT           | Grey
// #11/D11    |  PB3 | MOSI (hardware SPI) | Yellow
// #12/D12    |  PB4 | MISO (hardware SPI) | Green
// #13/D13    |  PB5 | SCK  (hardware SPI) | orange

//Arduino style pin defines
#define EVE_INT    (14)
#define EVE_PD_NOT (9)
#define EVE_CS_NOT (10)
#define DEBUG_LED  (3)

//Faster direct port access (specific to AVR)
#define CLR_EVE_PD_NOT        (digitalWrite(EVE_PD_NOT, LOW))
#define SET_EVE_PD_NOT        (digitalWrite(EVE_PD_NOT, HIGH))
#define CLR_EVE_CS_NOT        (digitalWrite(EVE_CS_NOT, LOW))
#define SET_EVE_CS_NOT        (digitalWrite(EVE_CS_NOT, HIGH))
#define CLR_MOSI              (digitalWrite(MOSI, LOW))
#define SET_MOSI              (digitalWrite(MOSI, HIGH))
#define CLR_MISO              (digitalWrite(MISO, LOW))
#define SET_MISO              (digitalWrite(MISO, HIGH))
#define CLR_SCK               (digitalWrite(SCK, LOW))
#define SET_SCK               (digitalWrite(SCK, HIGH))
#define CLR_DEBUG_LED         (digitalWrite(DEBUG_LED, LOW))
#define SET_DEBUG_LED         (digitalWrite(DEBUG_LED, HIGH))
//============================================================================
// These defines describe the circuit board and EVE accelerator. Typically
// you will want to use these directly and not mess with them.
#define EVE_CLOCK_SOURCE     (EVE_CLOCK_SOURCE_EXTERNAL)
#define EVE_CLOCK_MUL        (EVE_EXTERNAL_CLOCK_MUL_x6_72MHz)
#define EVE_CLOCK_SPEED      ((uint32_t)(EVE_CLOCK_MUL&0x1F)*(uint32_t)12000000)
#define EVE_TOUCH_TYPE       (EVE_TOUCH_CAPACITIVE)
#define EVE_TOUCH_CAP_DEVICE (EVE_CAP_DEV_FT5316)
#define EVE_PEN_UP_BUG_FIX   (0)
//Set this to force a calibration. Then you can copy that matrix from the
//console and use it to populate touch_transform[]
#define EVE_TOUCH_CAL_NEEDED (0)
// Define RGB output pins order, determined by PCB layout
#define LCD_SWIZZLE      (0x0)
// Define active edge of PCLK. Observed by scope:
//  0: Data is put out coincident with falling edge of the clock.
//     Rising edge of the clock is in the middle of the data.
//  1: Data is put out coincident with rising edge of the clock.
//     Falling edge of the clock is in the middle of the data.
#define LCD_PCLKPOL      (1)
// Pin drive settings for this particular board+display.
#if ((EVE_DEVICE == BT815) ||(EVE_DEVICE == BT816) ||(EVE_DEVICE == BT817) || (EVE_DEVICE == BT818))
// Newer EVE chips have great granularity.
// This table sets every pin, but you can delete lines
// where the default drive is acceptable
//
// Using a macro allows the pin drive to be defined here in the
// header file (which may be included multiple times across the
// project, but actually defined in the EVE_base.cpp file which
// is only compiled once.
#define PIN_DRIVE_TABLE const uint8_t Pin_Drive_Table[22] PROGMEM =\
  {\
  (EVE_PINDRIVE_GPIO_0_HR      | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_GPIO_1_HR      | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_GPIO_2_HR      | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_GPIO_3_HR      | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_DISP_LR        | EVE_PINDRIVE_LR_1p2mA),\
  (EVE_PINDRIVE_DE_LR          | EVE_PINDRIVE_LR_1p2mA),\
  (EVE_PINDRIVE_VSYNC_HSYNC_LR | EVE_PINDRIVE_LR_1p2mA),\
  (EVE_PINDRIVE_PCLK_LR        | EVE_PINDRIVE_LR_3p6mA),\
  (EVE_PINDRIVE_BACKLIGHT_LR   | EVE_PINDRIVE_LR_1p2mA),\
  (EVE_PINDRIVE_RGB_LR         | EVE_PINDRIVE_LR_1p2mA),\
  (EVE_PINDRIVE_AUDIO_L_HR     | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_INT_N_HR       | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_CTP_RST_N_HR   | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_CTP_SCL_HR     | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_CTP_SDA_HR     | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPI_DATA_HR    | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPIM_SCLK_HR   | EVE_PINDRIVE_HR_10mA),\
  (EVE_PINDRIVE_SPIM_SS_N_HR   | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPIM_MISO_HR   | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPIM_MOSI_HR   | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPIM_IO2_HR    | EVE_PINDRIVE_HR_5mA),\
  (EVE_PINDRIVE_SPIM_IO3_HR    | EVE_PINDRIVE_HR_5mA)\
  }
#endif // ((EVE_DEVICE == BT815) ||(EVE_DEVICE == BT816) ||(EVE_DEVICE == BT817) || (EVE_DEVICE == BT818))

// Spread Spectrum on RGB signals. Probably not a good idea at higher
// PCLK frequencies.
#define LCD_PCLK_CSPREAD (0)
//This is a 24-bit display, so no need to dither.
#define LCD_DITHER       (0)
// Pixel clock divisor (based on 72MHz internal clock)
//   0 = disable
//   1 = 60MHz (?)
//   2 = 36MHz
//   3 = 24MHz
//   4 = 18MHz
//   5 = 14.4MHz
//   6 = 12MHz
//   7 = 10.3MHz
//   8 = 9MHz
//   9 = 8MHz
//   10 = 7.2MHz
//   etc
// Our target is 33MHz.
// LCD_PCLK divisor of 2 gives us 36MHz
#define LCD_PCLK         (2)
//----------------------------------------------------------------------------
// Frame_Rate = 60Hz / 16.7mS
//----------------------------------------------------------------------------
// Horizontal timing (minimum values from ILI6122_SPEC_V008.pdf page 45)
// Target 60Hz frame rate, using the largest possible line time in order to
// maximize the time that the EVE has to process each line.
#define HPX   (800)    // Horizontal Pixel Width
#define HSW   (4)      // Horizontal Sync Width (1~40)
#define HBP   (8)      // Horizontal Back Porch (must be 46, includes HSW)
#define HFP   (8)      // Horizontal Front Porch (16~210~354)
#define HPP   (178)    // Horizontal Pixel Padding (tot=863: 862~1056~1200)
                       // EVE needs at least 1 here
// Define the constants needed by the EVE based on the timing
// Active width of LCD display
#define LCD_WIDTH   (HPX)
// Start of horizontal sync pulse
#define LCD_HSYNC0  (HFP)
// End of horizontal sync pulse
#define LCD_HSYNC1  (HFP+HSW)
// Start of active line
#define LCD_HOFFSET (HFP+HSW+HBP)
// Total number of clocks per line
#define LCD_HCYCLE  (HPX+HFP+HSW+HBP+HPP)
//----------------------------------------------------------------------------
// Vertical timing (minimum values from ILI6122_SPEC_V008.pdf page 46)
#define VLH   (480)   // Vertical Line Height
#define VS    (4)     // Vertical Sync (in lines)  (1~20)
#define VBP   (8)     // Vertical Back Porch (must be 23, includes VS)
#define VFP   (8)     // Vertical Front Porch (7~22~147)
#define VLP   (1)     // Vertical Line Padding (tot=511: 510~525~650)
                      // EVE needs at least 1 here
// Define the constants needed by the EVE based on the timing
// Active height of LCD display
#define LCD_HEIGHT  (VLH)
// Start of vertical sync pulse
#define LCD_VSYNC0  (VFP)
// End of vertical sync pulse
#define LCD_VSYNC1  (VFP+VS)
// Start of active screen
#define LCD_VOFFSET (VFP+VS+VBP)
// Total number of lines per screen
#define LCD_VCYCLE  (VLH+VFP+VS+VBP+VLP)
//============================================================================
// Throw an error if the controller does not match the touch type.
#if ((EVE_TOUCH_TYPE == EVE_TOUCH_RESISTIVE) && \
     ((EVE_DEVICE == FT801) || \
      (EVE_DEVICE == FT811) || \
      (EVE_DEVICE == FT813) || \
      (EVE_DEVICE == FT815) || \
      (EVE_DEVICE == FT817)))
  #error Cannot specify EVE_TOUCH_RESISTIVE for an EVE_DEVICE that only supports capacitive touch.
#endif
#if ((EVE_TOUCH_TYPE == EVE_TOUCH_CAPACITIVE) && \
     ((EVE_DEVICE == FT800) || \
      (EVE_DEVICE == FT810) || \
      (EVE_DEVICE == FT812) || \
      (EVE_DEVICE == FT816) || \
      (EVE_DEVICE == FT818)))
  #error Cannot specify EVE_TOUCH_CAPACITIVE for an EVE_DEVICE that only supports resistive touch.
#endif
//============================================================================
// Remotely control the backlight (debug only).
#if (DEBUG_LEVEL != DEBUG_NONE)
#define REMOTE_BACKLIGHT_DEBUG (0)
#endif // (DEBUG_LEVEL != DEBUG_NONE)
//============================================================================
#endif // __CFA10100_DEFINES_H__
