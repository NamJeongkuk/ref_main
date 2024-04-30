/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTSTATE_H
#define CONVERTIBLECOMPARTMENTSTATE_H

#include <stdint.h>
#include "EventSubscription.h"
#include "I_DataModel.h"
#include "ConvertibleCompartmentData.h"

typedef struct
{
   Erd_t featurePanResolvedSetpointErd; // SetpointVotedTemperature_t
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
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

void ConvertibleCompartmentState_Init(
   ConvertibleCompartmentState_t *instance,
   I_DataModel_t *dataModel,
   const ConvertibleCompartmentStateConfig_t *config);

#endif
