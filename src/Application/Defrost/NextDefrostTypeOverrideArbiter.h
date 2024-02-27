/*!
 * @file
 * @brief This module determine the next defrost type override after counting number of fresh food defrosts
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NEXTDEFROSTTYPEARBITER_H
#define NEXTDEFROSTTYPEARBITER_H

#include "EventSubscription.h"
#include "DefrostData.h"
#include "EnhancedSabbathData.h"
#include "I_DataModel.h"

typedef struct
{
   Erd_t nextDefrostTypeOverrideErd; // DefrostType_t
   Erd_t defrostingErd; // bool
   Erd_t numberOfSecondaryOnlyDefrostsErd; // uint8_t
   Erd_t numberOfSecondaryOnlyDefrostsBeforeAFullDefrostErd; // uint8_t
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t currentDefrostTypeErd; // DefrostType_t
   Erd_t freezerFilteredTemperatureTooWarmAtPowerUpErd; // bool
} NextDefrostTypeOverrideArbiterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const DefrostData_t *defrostData;
      const EnhancedSabbathData_t *enhancedSabbathData;
      const NextDefrostTypeOverrideArbiterConfig_t *config;
   } _private;
} NextDefrostTypeOverrideArbiter_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 */
void NextDefrostTypeOverrideArbiter_Init(
   NextDefrostTypeOverrideArbiter_t *instance,
   I_DataModel_t *dataModel,
   const NextDefrostTypeOverrideArbiterConfig_t *config);

#endif
