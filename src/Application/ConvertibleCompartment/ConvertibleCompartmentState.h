/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENT_H
#define CONVERTIBLECOMPARTMENT_H

#include <stdint.h>
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "ConvertibleCompartmentData.h"

typedef struct
{
   Erd_t convertibleCompartmentResolvedSetpointErd;
   Erd_t convertibleCompartmentStateErd;
} ConvertibleCompartmentStateConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onSetpointChanged;
      const ConvertibleCompartmentStateConfig_t *config;
      const ConvertibleCompartmentData_t *convertibleCompartmentData;
   } _private;
} ConvertibleCompartmentState_t;

void ConvertibleCompartmentState_Init(ConvertibleCompartmentState_t *instance, I_DataModel_t *dataModel, const ConvertibleCompartmentStateConfig_t *config);

#endif
