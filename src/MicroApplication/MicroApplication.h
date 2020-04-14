/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROAPPLICATION_H
#define MICROAPPLICATION_H

#include "ApplianceApiPlugin.h"
#include "I_TinyDataSource.h"

typedef struct
{
   struct
   {
      ApplianceApiPlugin_t ApplianceApiPlugin;
   } _private;
} MicroApplication_t;

/*!
 * @param instance
 * @param dataSource
 */
void MicroApplication_Init(MicroApplication_t *instance, I_TinyDataSource_t *dataSource);

#endif
