#include "wifi.h"
#include "cli.h"

#ifdef _USE_HW_CLI
static void cliWifi(cli_args_t *args);
#endif

static bool is_init = false;





bool wifiInit(void)
{
  bool ret = false;

  if (cyw43_arch_init() == PICO_OK) 
  {
    logPrintf("WiFi OK\n");
    ret = true;
  }
  else
  {
    logPrintf("WiFi Fail\n");
    ret = false;
  }

  is_init = ret;

#ifdef _USE_HW_CLI
  cliAdd("wifi", cliWifi);
#endif
  return ret;
}

bool wifiIsInit(void)
{
  return is_init;
}

#ifdef _USE_HW_CLI
typedef struct 
{
  uint8_t max;
  uint8_t len;
  cyw43_ev_scan_result_t list[16];
} wifi_scan_ret_t;

static int scanResult(void *env, const cyw43_ev_scan_result_t *result) 
{
  if (result) 
  {
    wifi_scan_ret_t *p_scan_ret = (wifi_scan_ret_t *)env;
    bool is_add = true;

    for (int i=0; i<p_scan_ret->len && i<p_scan_ret->max; i++)
    {
      if (strcmp((const char *)result->ssid, (const char *)p_scan_ret->list[i].ssid) == 0)
      {
        is_add = false;
        break;
      }
    }
    if (is_add == true && p_scan_ret->len < p_scan_ret->max)
    {
      p_scan_ret->list[p_scan_ret->len] = *result;
      p_scan_ret->len++;
    }
  }
  return 0;
}

void cliWifi(cli_args_t *args)
{
  bool ret = false;


  if (args->argc == 1 && args->isStr(0, "info"))
  {
    cliPrintf("WiFi Init : %s\n", is_init ? "OK" : "Fail");
    ret = true;
  }

  if (args->argc == 1 && args->isStr(0, "scan"))
  {
    bool scan_in_progress = false;
    cyw43_wifi_scan_options_t scan_options = {0};
    wifi_scan_ret_t scan_list;
    
    scan_list.max = 16;
    scan_list.len = 0;

    cyw43_arch_enable_sta_mode();
    while(cliKeepLoop())
    {
      if (!scan_in_progress) 
      {
        
        int err = cyw43_wifi_scan(&cyw43_state, &scan_options, &scan_list, scanResult);
        if (err == 0) 
        {
          logPrintf("wifi scan begin\n");
          scan_in_progress = true;
        } 
        else 
        {
          logPrintf("Failed to start scan: %d\n", err);
        }
      } 
      else if (!cyw43_wifi_scan_active(&cyw43_state)) 
      {
        scan_in_progress = false; 
        logPrintf("wifi scan end\n");
        break;
      }
      delay(10);
    }

    for (int i=0; i<scan_list.len; i++)
    {
      const char *auth_str[] = {"OPEN", "WPA_TKIP_PSK", "WPA2_AES_PSK", "WPA2_MIXED_PSK"};

      logPrintf("%-2d -> ssid: %-32s rssi: %4d chan: %3d   %s\n",
          i, 
          scan_list.list[i].ssid, 
          scan_list.list[i].rssi, 
          scan_list.list[i].channel,
          auth_str[(scan_list.list[i].auth_mode & 0xFF)>>1]
          );
    }
    ret = true;
  }

  if (ret == false)
  {
    cliPrintf("wifi info\n");
    cliPrintf("wifi scan\n");
  }
}
#endif