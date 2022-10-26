/*
 * hw.c
 *
 *  Created on: Jun 13, 2021
 *      Author: baram
 */

#include "hw.h"



static const firm_ver_t boot_ver __attribute__((section(".version"))) = 
{
  .magic_number = VERSION_MAGIC_NUMBER,
  .version_str  = _DEF_FIRMWATRE_VERSION,
  .name_str     = _DEF_BOARD_NAME,
};





bool hwInit(void)
{  

  cliInit();
  resetInit();
  logInit();
  uartInit();
  uartOpen(_DEF_UART1, 115200);
  logOpen(_DEF_UART1, 115200);
  
  flashInit();

  wifiInit();
  ledInit();

  logPrintf("[ Bootloader Begin... ]\r\n");
  logPrintf("Booting..Name \t\t: %s\r\n", _DEF_BOARD_NAME);
  logPrintf("Booting..Ver  \t\t: %s\r\n", _DEF_FIRMWATRE_VERSION);
  logPrintf("Clk sys  \t\t: %d\r\n", clock_get_hz(clk_sys));
  logPrintf("Clk peri \t\t: %d\r\n", clock_get_hz(clk_peri));
  logPrintf("Clk usb  \t\t: %d\r\n", clock_get_hz(clk_usb));
  logPrintf("Clk adc  \t\t: %d\r\n", clock_get_hz(clk_adc));
  logPrintf("Clk rtc  \t\t: %d\r\n", clock_get_hz(clk_rtc));
  logPrintf("Clk timer\t\t: %d\r\n", clock_get_hz(clk_ref));
  logPrintf("\n");
  logPrintf("Reset Mode\t\t: %s\r\n", resetGetBootModeMsg());
  logPrintf("Reset Count\t\t: %d\r\n", resetGetCount());
  logPrintf("Boot Ver Addr\t\t: 0x%X\r\n", (int)&boot_ver);
  logPrintf("\n");


  return true;
}


void jumpToFw(void)
{
  void (**jump_func)(void) = (void (**)(void))(FLASH_ADDR_FW_VECTOR + 4); 


  bspDeInit();
  __set_MSP(*(uint32_t *)(FLASH_ADDR_FW_VECTOR));
  scb_hw->vtor = FLASH_ADDR_FW_VECTOR;


  resetSetBootMode(RESET_MODE_FW);

  (*jump_func)();
}