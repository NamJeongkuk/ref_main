/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "ApplianceApiPlugin.h"
#include "McCommunicationPlugin.h"
#include "CapTouchPlugin.h"

static inline void Application_Init(
   I_DataSource_t *dataSource,
   I_DataSource_t *externalDataSource,
   I_Gea2PacketEndpoint_t *gea2PacketEndpoint)
{
   ApplianceApiPlugin_Init(dataSource);
   McCommunicationPlugin_Init(dataSource, externalDataSource, gea2PacketEndpoint);
   CapTouchPlugin_Init(dataSource);
}

#endif
