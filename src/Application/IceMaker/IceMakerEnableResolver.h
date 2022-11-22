/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERENABLERESOLVER_H
#define ICEMAKERENABLERESOLVER_H

#include "I_DataModel.h"
#include "ErdLogicService.h"

typedef struct
{
   ErdLogicService_t logicService;
} IceMakerEnableResolver_t;

void IceMakerEnableResolver_Init(
   IceMakerEnableResolver_t *instance,
   I_DataModel_t *dataModel,
   const ErdLogicServiceConfiguration_t *erdLogicServiceConfiguration);

#endif
