/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentStatePlugin.h"
#include "SystemErds.h"

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
   ConvertibleCompartmentState_Init(&instance->instance, dataModel, &config);
}
