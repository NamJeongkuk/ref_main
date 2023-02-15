/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentStatePlugin.h"
#include "SystemErds.h"
#include "Constants_Binary.h"
#include "uassert.h"

static const ConvertibleCompartmentStateConfig_t config = {
   .convertibleCompartmentResolvedSetpointErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentState
};

/*!
 * Initializes State plugin which sets ConvertibleCompartmentState Erd based on ConvertibleCompartmentSetpoint
 * @param dataModel The data model.
 * @param config
 */
void ConvertibleCompartmentStatePlugin_Init(ConvertibleCompartmentStatePlugin_t *instance, I_DataModel_t *dataModel)
{
   bool setpointResolverReady;
   DataModel_Read(
      dataModel,
      Erd_SetpointResolverReady,
      &setpointResolverReady);

   uassert(setpointResolverReady);

   ConvertibleCompartmentState_Init(&instance->instance, dataModel, &config);

   DataModel_Write(
      dataModel,
      Erd_ConvertibleCompartmentStateResolverReady,
      set);
}
