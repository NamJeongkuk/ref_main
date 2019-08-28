/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYAPPLICATION_H
#define TINYAPPLICATION_H

#include "SwitchedLedPlugin.h"
#include "I_TinyDataSource.h"

typedef struct
{
   struct
   {
      SwitchedLedPlugin_t switchedLedPlugin;
   } _private;
} TinyApplication_t;

/*!
 * @param instance
 * @param dataSource
 */

void TinyApplication_Init(TinyApplication_t *instance, I_TinyDataSource_t *dataSource);

#endif
