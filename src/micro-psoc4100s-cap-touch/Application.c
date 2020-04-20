/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Application.h"
#include "ApplianceApiPlugin.h"
#include "CapTouchPlugin.h"

void Application_Init(I_TinyDataSource_t *dataSource)
{
   ApplianceApiPlugin_Init(dataSource);
   CapTouchPlugin_Init(dataSource);
}
