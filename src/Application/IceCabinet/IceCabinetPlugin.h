/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETPLUGIN_H
#define ICECABINETPLUGIN_H

#include "I_DataModel.h"
#include "IceCabinetGridPlugin.h"

typedef struct
{
   IceCabinetGridPlugin_t iceCabinetGridPlugin;
} IceCabinetPlugin_t;

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
