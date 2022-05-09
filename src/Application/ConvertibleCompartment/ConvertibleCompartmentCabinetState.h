/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTCABINETSTATE_H
#define CONVERTIBLECOMPARTMENTCABINETSTATE_H

#include <stdint.h>
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "ConvertibleCompartmentCabinetData.h"

typedef struct
{
   Erd_t convertibleCompartmentResolvedSetpointErd;
   Erd_t convertibleCompartmentStateErd;
} ConvertibleCompartmentCabinetStateConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onSetpointChanged;
      const ConvertibleCompartmentCabinetStateConfig_t *config;
      const ConvertibleCompartmentCabinetData_t *convertibleCompartmentCabinetData;
   } _private;
} ConvertibleCompartmentCabinetState_t;

void ConvertibleCompartmentCabinetState_Init(ConvertibleCompartmentCabinetState_t *instance, I_DataModel_t *dataModel, const ConvertibleCompartmentCabinetStateConfig_t *config);

#endif
