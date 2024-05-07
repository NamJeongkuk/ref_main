/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETFANPLUGIN_H
#define ICECABINETFANPLUGIN_H

#include "I_DataModel.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      ErdResolver_t fanSpeedVoteResolver;
   } _private;
} IceCabinetFanPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void IceCabinetFanPlugin_Init(
   IceCabinetFanPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
