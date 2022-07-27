/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROST_H
#define DEFROST_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "SabbathData.h"
#include "GridData.h"
#include "EvaporatorData.h"
#include "Timer.h"
#include "Hsm.h"

typedef struct
{
   Erd_t defrostHsmStateErd; // DefrostHsmState_t
   Erd_t freezerFilteredTemperatureResolvedErd; // TemperatureDegFx100_t
   Erd_t calculatedGridLinesErd; // CalculatedGridLines_t
   Erd_t defrostStateErd; // DefrostState_t
   Erd_t numberOfFreezerAbnormalDefrostCyclesErd; // uint16_t
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t freezerAbnormalDefrostCycleCountErd; // uint16_t
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t freezerDefrostCycleCountErd; // uint16_t
   Erd_t defrostReadyTimerIsSatisfied; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t powerUpDelayTimer;
      Timer_t periodicTimer;
      EventSubscription_t dataModelSubscription;
      uint32_t prechillRunCounterInMinutes;
      const DefrostConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      const GridData_t *gridParametricData;
      const EvaporatorData_t *evaporatorParametricData;
   } _private;
} Defrost_t;

void Defrost_Init(
   Defrost_t *instance,
   I_DataModel_t *dataModel,
   const DefrostConfiguration_t *defrostConfig);

#endif
