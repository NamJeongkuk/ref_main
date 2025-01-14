/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DAMPERFREEZEPREVENTION_H
#define DAMPERFREEZEPREVENTION_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "Timer.h"
#include "DamperData.h"
#include "TemperatureDegFx100.h"
#include "Fsm.h"

#define ConvertDegFx100 100

typedef struct
{
   Erd_t damperHeaterVoteErd; // PercentageDutyCycleVote_t
   Erd_t sourceThermistorIsValidResolvedErd; // bool
   Erd_t targetThermistorIsValidResolvedErd; // bool
   Erd_t sourceFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t targetFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t damperCurrentPositionErd; // DamperPostion_t
   Erd_t timerModuleErd; // TimerModule_t
   Erd_t damperFreezePreventionFsmStateErd; // DamperFreezePreventionFsmState_t
   Erd_t damperFullCycleRequestErd; // bool
} DamperFreezePreventionConfiguration_t;

typedef struct
{
   struct
   {
      Fsm_t fsm;
      I_DataModel_t *dataModel;
      Timer_t timer;
      TemperatureDegFx100_t startingTemperature;
      EventSubscription_t dataModelSubscription;
      const DamperFreezePreventionConfiguration_t *configuration;
      const DamperData_t *damperData;
   } _private;
} DamperFreezePrevention_t;

/*!
 *
 * @param instance
 * @param dataModel
 * @param config
 * @param damperData
 */
void DamperFreezePrevention_Init(
   DamperFreezePrevention_t *instance,
   I_DataModel_t *dataModel,
   const DamperFreezePreventionConfiguration_t *config,
   const DamperData_t *damperData);

#endif
