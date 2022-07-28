/*!
 * @file
 * @brief plugin for fans
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANPLUGIN_H
#define FANPLUGIN_H

#include "I_DataModel.h"
#include "FanSpeedResolver.h"
#include "FanId.h"

typedef struct
{
   struct
   {
      FanSpeedResolver_t fanSpeedResolver[FanId_NumerOfFans];
   } _private;
} FanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void FanPlugin_Init(
   FanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
