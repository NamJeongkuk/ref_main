/*!
 * @file
 * @brief Aluminum mold ice maker state machine
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALUMINUMMOLDICEMAKER_H
#define ALUMINUMMOLDICEMAKER_H

#include "I_DataModel.h"
#include "Hsm.h"

typedef struct
{
   Erd_t aluminumMoldIceMakerHsmStateErd; // AluminumMoldIceMakerHsmState_t
} AluminumMoldIceMakerConfig_t;

typedef struct
{
   struct
   {
      Hsm_t hsm;
      I_DataModel_t *dataModel;
      const AluminumMoldIceMakerConfig_t *config;
   } _private;
} AluminumMoldIceMaker_t;

void AluminumMoldIceMaker_Init(
   AluminumMoldIceMaker_t *instance,
   I_DataModel_t *dataModel,
   const AluminumMoldIceMakerConfig_t *config);

#endif
