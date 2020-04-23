/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "WiFiPlugin.h"
#include "TinyWiFiService.h"

static TinyWiFiService_t wifiService;

static const TinyWiFiServiceConfiguration_t configuration = {

}

void WiFiPlugin_Init(I_TinyDataSource_t * dataSource)
{
   TinyWiFiService_Init(&wifiService,
}
