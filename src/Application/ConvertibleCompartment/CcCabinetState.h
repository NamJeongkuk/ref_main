/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CCCABINETSTATE_H
#define CCCABINETSTATE_H

#include <stdint.h>
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "CcCabinetData.h"

enum
{
   CcCabinetState_FreshFood,
   CcCabinetState_Freezer
};

typedef struct
{
   Erd_t ccResolvedSetpointErd;
   Erd_t ccStateErd;
} CcCabinetStateConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onSetpointChanged;
      const CcCabinetStateConfig_t *config;
      const CcCabinetData_t *ccCabinetData;
   } _private;
} CcCabinetState_t;

void CcCabinetState_Init(CcCabinetState_t *instance, I_DataModel_t *dataModel, const CcCabinetStateConfig_t *config);

#endif
