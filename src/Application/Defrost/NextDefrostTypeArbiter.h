/*!
 * @file
 * @brief This module determine the next defrost type after counting number of fresh food defrosts
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
   Erd_t nextDefrostTypeErd; // DefrostType_t
   Erd_t defrostingErd; // bool
   Erd_t freshFoodDefrostCountErd; // uint8_t
   Erd_t numberOfFreshFoodDefrostsBeforeAFreezerDefrostErd; // uint8_t
   Erd_t enhancedSabbathModeErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentStateType_t
   Erd_t currentDefrostTypeErd; // DefrostType_t
   Erd_t clearedDefrostEepromStartupErd; // bool
   Erd_t freezerFilteredTemperatureTooWarmAtPowerUpErd; // bool
} NextDefrostTypeArbiterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const DefrostData_t *defrostData;
      const EnhancedSabbathData_t *enhancedSabbathData;
      const NextDefrostTypeArbiterConfig_t *config;
   } _private;
} NextDefrostTypeArbiter_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 */
void NextDefrostTypeArbiter_Init(
   NextDefrostTypeArbiter_t *instance,
   I_DataModel_t *dataModel,
   const NextDefrostTypeArbiterConfig_t *config);

#endif
