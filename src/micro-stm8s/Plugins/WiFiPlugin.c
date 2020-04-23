/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WiFiPlugin.h"
#include "TinyWiFiService.h"
#include "MicroSystemErds.h"

static TinyWiFiService_t wifiService;

static const TinyWiFiServiceConfiguration_t configuration = {
   Erd_WiFiServiceRequest,
   Erd_WiFiServiceState,
   Erd_WiFiServiceConnectionEstablished
};

void WiFiPlugin_Init(I_TinyDataSource_t *dataSource, I_TinyGea2Interface_t *gea2Interface)
{
   TinyTimerModule_t *timerModule;
   TinyDataSource_Read(dataSource, Erd_TimerModule, &timerModule);
   TinyWiFiService_Init(&wifiService, dataSource, gea2Interface, timerModule, &configuration);
}
