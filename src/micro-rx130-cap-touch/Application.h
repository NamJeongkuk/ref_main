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
#include "CapTouchDebugPlugin.h"

static inline void Application_Init(
   I_TinyDataSource_t *dataSource,
   I_TinyDataSource_t *externalDataSource,
   I_TinyGea2Interface_t *gea2Interface)
{
   ApplianceApiPlugin_Init(dataSource);
   McCommunicationPlugin_Init(dataSource, externalDataSource, gea2Interface);
   CapTouchPlugin_Init(dataSource);
   CapTouchDebugPlugin_Init(dataSource);
}

#endif
