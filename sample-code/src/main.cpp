#include <Arduino.h>
#include <SPI.h>
#include <stdarg.h>
// Definitions for our circuit board and display.
#include "CFA10100_defines.h"

// The very simple EVE library files
#include "EVE_base.h"
#include "EVE_draw.h"

// Our demonstrations of various EVE functions
#include "dash.h"

Dash dashboard{};
uint16_t FWo;

void setup()
{
#if (DEBUG_LEVEL != DEBUG_NONE)
  // Initialize UART for debugging messages
  Serial.begin(115200);
#endif // (DEBUG_LEVEL != DEBUG_NONE)
  DBG_STAT("Begin\n");

  // Initialize GPIO port states
  SET_EVE_CS_NOT; // Set CS# high to start - SPI inactive
  SET_EVE_PD_NOT; // Set PD# high to start

  // Initialize port directions
  pinMode(EVE_INT, INPUT_PULLUP); // EVE interrupt output
  pinMode(EVE_PD_NOT, OUTPUT);    // EVE Power Down (reset) input
  pinMode(EVE_CS_NOT, OUTPUT);    // EVE SPI bus CS# input
  pinMode(DEBUG_LED, OUTPUT);     // Optional pin for LED/oscilloscope debugging.

  // Initialize SPI
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  DBG_GEEK("SPI initialzed to: 8MHz\n");

  // See if we can find the FTDI/BridgeTek EVE processor
  if (0 != EVE_Initialize())
  {
    DBG_STAT(
        "Failed to initialize %s8%02X. Stopping.\n",
        EVE_DEVICE < 0x14 ? "FT" : "BT",
        EVE_DEVICE);
    while (1)
      ;
  }
  else
  {
    DBG_STAT("%s8%02X initialized.\n", EVE_DEVICE < 0x14 ? "FT" : "BT", EVE_DEVICE);
  }

  DBG_GEEK("Loop initialization.\n");

  // Get the current write pointer from the EVE

  FWo = EVE_REG_Read_16(EVE_REG_CMD_WRITE);
  DBG_GEEK("Initial Offset Read: 0x%04X = %u\n", FWo, FWo);

  // Keep track of the RAM_G memory allocation
  uint32_t RAM_G_Unused_Start;
  RAM_G_Unused_Start = 0;
  DBG_GEEK("Initial RAM_G: 0x%08lX = %lu\n", RAM_G_Unused_Start, RAM_G_Unused_Start);

  // We need to keep track of the bitmap handles and where they are used.
  //
  // By default, bitmap handles 16 to 31 are used for built-in font and 15
  // is used as scratch bitmap handle by co-processor engine commands
  // CMD_GRADIENT, CMD_BUTTON and CMD_KEYS.
  //
  // For whatever reason, I am going to allocate handles from 14 to 0.
  uint8_t next_bitmap_handle_available;
  next_bitmap_handle_available = 14;

  DBG_GEEK("EVE_Initialize_Flash() . . . ");
  FWo = EVE_Initialize_Flash(FWo);
  DBG_GEEK("done.\n");

  uint8_t flash_status;
  flash_status = EVE_REG_Read_8(EVE_REG_FLASH_STATUS);
  DBG_GEEK_Decode_Flash_Status(flash_status);

  DBG_GEEK("Not programming flash.\n");

  DBG_STAT("Initializing Dash...");
  dashboard.Initialize();
  DBG_STAT(" done.\n");

  StartEVEExecution(FWo);
  DBG_STAT("Initialization complete, entering main loop.\n");
}

void RunDisplay()
{
  static std::tuple<uint16_t, bool> execution_result;
  execution_result = RunEVEExecution();
  if (std::get<1>(execution_result))
  {
    FWo = std::get<0>(execution_result);
    // Start the display list
    FWo = EVE_Cmd_Dat_0(FWo, (EVE_ENC_CMD_DLSTART));

    // Set default clear color
    if (dashboard.mode)
    {
      FWo = dashboard.UpdateBackground(FWo, 0, 0, 0);
    }
    else
    {
      FWo = dashboard.UpdateBackground(FWo, 255, 255, 255);
    }

    // Clear the screen
    FWo = EVE_Cmd_Dat_0(FWo, EVE_ENC_CLEAR(1, 1, 1));

    // //Fill background with white
    // FWo = EVE_Filled_Rectangle(FWo, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1);

    FWo = dashboard.AddToDisplayList(FWo);

    // Show the display list
    FWo = EVE_Cmd_Dat_0(FWo, EVE_ENC_DISPLAY());
    FWo = EVE_Cmd_Dat_0(FWo, EVE_ENC_CMD_SWAP);
    EVE_REG_Write_16(EVE_REG_CMD_WRITE, (FWo));
    StartEVEExecution(FWo);
  }
}

void loop()
{
  dashboard.GetCAN();
  RunDisplay();
  // FWo = Wait_for_EVE_Execution_Complete(FWo);
}
