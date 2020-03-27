/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NANOAPPLICATION_H
#define NANOAPPLICATION_H

#include "SwitchedLedPlugin.h"
#include "TinyErdStreamSender.h"
#include "I_TinyDataSource.h"

typedef struct
{
   struct
   {
      SwitchedLedPlugin_t switchedLedPlugin;
   } _private;
} NanoApplication_t;

/*!
 * @param instance
 * @param dataSource
 */
void NanoApplication_Init(NanoApplication_t *instance, I_TinyDataSource_t *dataSource);

#endif
