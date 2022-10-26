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
static int scanResult(void *env, const cyw43_ev_scan_result_t *result) 
{
  if (result) 
  {
    logPrintf("ssid: %-32s rssi: %4d chan: %3d mac: %02x:%02x:%02x:%02x:%02x:%02x sec: %u\n",
          result->ssid, result->rssi, result->channel,
          result->bssid[0], result->bssid[1], result->bssid[2], result->bssid[3], result->bssid[4], result->bssid[5],
          result->auth_mode);
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
    
    
    cyw43_arch_enable_sta_mode();
    while(cliKeepLoop())
    {
      if (!scan_in_progress) 
      {
        
        int err = cyw43_wifi_scan(&cyw43_state, &scan_options, NULL, scanResult);
        if (err == 0) 
        {
          logPrintf("\nPerforming wifi scan\n");
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
        break;
      }
      delay(10);
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