/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MicroApplication.h"
#include "ApplianceApiPlugin.h"
#include "WiFiPlugin.h"

void MicroApplication_Init(I_TinyDataSource_t *dataSource, I_TinyGea2Interface_t *gea2Interface)
{
   ApplianceApiPlugin_Init(dataSource);
   WiFiPlugin_Init(dataSource, gea2Interface);
}
