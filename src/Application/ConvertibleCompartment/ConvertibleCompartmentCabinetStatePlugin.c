/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ConvertibleCompartmentCabinetStatePlugin.h"
#include "SystemErds.h"

static const ConvertibleCompartmentCabinetStateConfig_t config = {
   .convertibleCompartmentResolvedSetpointErd = Erd_ConvertibleCompartmentSetpoint_ResolvedVote,
   .convertibleCompartmentStateErd = Erd_ConvertibleCompartmentCabinetState
};

/*!
 * Initializes CabinetState plugin which sets ConvertibleCompartmentCabinetState Erd based on ConvertibleCompartmentSetpoint
 * @param dataModel The data model.
 * @param config
 */
void ConvertibleCompartmentCabinetStatePlugin_Init(ConvertibleCompartmentCabinetStatePlugin_t *instance, I_DataModel_t *dataModel)
{
   ConvertibleCompartmentCabinetState_Init(&instance->instance, dataModel, &config);
}
