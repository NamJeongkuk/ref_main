/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceCabinetPlugin.h"

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel)
{
   // TODO: Plug in gamma calculator before this
   IceCabinetGridPlugin_Init(&instance->iceCabinetGridPlugin, dataModel);
}
