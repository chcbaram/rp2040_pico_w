/*
 * led.c
 *
 *  Created on: 2021. 6. 14.
 *      Author: baram
 */


#include "led.h"

#ifdef _USE_HW_LED


typedef struct
{
  uint32_t    pin;
  uint8_t     on_state;
  uint8_t     off_state;
  uint8_t     value;
} led_tbl_t;


static bool is_init = false;
led_tbl_t led_tbl[LED_MAX_CH];





bool ledInit(void)
{

  is_init = true;

  for (int i=0; i<LED_MAX_CH; i++)
  {
    ledOff(i);
  }

  return true;
}

void ledOn(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;
  if (is_init == false) return;

  led_tbl[ch].value = 1;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
}

void ledOff(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;
  if (is_init == false) return;

  led_tbl[ch].value = 0;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
}

void ledToggle(uint8_t ch)
{
  if (ch >= LED_MAX_CH) return;
  if (is_init == false) return;

  led_tbl[ch].value ^= 1;
  cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_tbl[ch].value);
}

#endif