/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CcCabinetStatePlugin.h"
#include "SystemErds.h"

static const CcCabinetStateConfig_t config = {
   .ccResolvedSetpointErd = Erd_CcSetpoint_ResolvedVote,
   .ccStateErd = Erd_CcCabinetState
};

/*!
 * Initializes CabinetState plugin which sets CcCabinetState Erd based on CcSetpoint
 * @param dataModel The data model.
 * @param config
 */
void CcCabinetStatePlugin_Init(CcCabinetStatePlugin_t *instance, I_DataModel_t *dataModel)
{
   CcCabinetState_Init(&instance->instance, dataModel, &config);
}
