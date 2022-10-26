#ifndef WIFI_H_
#define WIFI_H_


#ifdef __cplusplus
extern "C" {
#endif

#include "hw_def.h"


#ifdef _USE_HW_WIFI



bool wifiInit(void);
bool wifiInInit(void);

#endif

#ifdef __cplusplus
}
#endif


#endif