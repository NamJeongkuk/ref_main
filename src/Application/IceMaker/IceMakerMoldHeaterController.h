/*!
 * @file
 * @brief Implementation of an Ice Maker Heater Mold controller that votes for an Ice Maker Mold Heater State
 *        based on a requested temperature bound. If the Ice Maker Mold temperature is between the requested bound the
 *        Heater will be voted on. Otherwise, it is voted off.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOLDHEATERCONTROLLER_H
#define ICEMAKERMOLDHEATERCONTROLLER_H

#include "I_DataModel.h"
#include "AluminumMoldIceMakerData.h"
#include "Timer.h"
#include "Fsm.h"

typedef struct
{
   Erd_t iceMakerMoldHeaterControlRequestErd; // IceMakerMoldHeaterControlRequest_t
   Erd_t iceMakerMoldHeaterHarvestVoteErd; // HeaterVotedState_t
   Erd_t iceMakerMoldFilteredTemperatureErd; // TemperatureDegFx100_t
   Erd_t timerModuleErd; // TimerModule_t *
} IceMakerMoldHeaterControllerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Timer_t heaterTimer;
      EventSubscription_t onDataModelChangeSubscription;
      Fsm_t fsm;
      const IceMakerMoldHeaterControllerConfig_t *config;
      const AluminumMoldIceMakerHarvestData_t *harvestData;
   } _private;
} IceMakerMoldHeaterController_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param harvestData
 */
void IceMakerMoldHeaterController_Init(
   IceMakerMoldHeaterController_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerMoldHeaterControllerConfig_t *config,
   const AluminumMoldIceMakerHarvestData_t *harvestData);

#endif
