/*!
 * @file
 * @brief Determines if system is ready to defrost
 * based on the compressor on time and door accelerations
 * compared to the time between defrosts. Time between defrosts is
 * determined by various system conditions and is either a minimum
 * time or a maximum time. If time between defrosts
 * is maximum. Compressor on time must reach at least the
 * minimum time before reaching the maximum time.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef READYTODEFROSTIMPROVED_H
#define READYTODEFROSTIMPROVED_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "Hsm.h"
#include "Timer.h"

typedef struct
{
   Erd_t doorIsOpenErd; // bool
   Erd_t doorAccelerationErd; // uint32_t
   size_t offsetInParametricForDoorFactor;
} ReadyToDefrostDoorConfiguration_t;

typedef struct
{
   Erd_t compressorIsOnErd; // bool
   Erd_t defrostCompressorOnTimeInSecondsErd; // uint32_t
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpReadyErd; // bool
   Erd_t freezerFilteredTemperatureWasTooWarmAtPowerUpErd; // bool
   Erd_t useMinimumReadyToDefrostTimeAndResetDefrostCountsErd; // bool
   Erd_t invalidFreezerEvaporatorThermistorDuringDefrostErd; // bool
   Erd_t freshFoodDefrostWasAbnormalErd; // bool
   Erd_t freezerDefrostWasAbnormalErd; // bool
   Erd_t convertibleCompartmentDefrostWasAbnormalErd; // bool
   Erd_t hasConvertibleCompartmentErd; // bool
   Erd_t eepromClearedErd; // bool
   Erd_t waitingForDefrostTimeInSecondsErd; // uint32_t
   Erd_t waitingToDefrostErd; // bool
   Erd_t readyToDefrostErd; // bool
   Erd_t timeBetweenDefrostsInMinutesErd; // uint32_t
   Erd_t readyToDefrostHsmStateErd; // ReadyToDefrostHsmState_t
   const ReadyToDefrostDoorConfiguration_t *doorsConfiguration;
   uint8_t numberOfDoors;
} ReadyToDefrostImprovedConfiguration_t;

typedef struct
{
   struct
   {
      bool useMinimumTimeBetweenDefrosts;
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t timer;
      EventSubscription_t dataModelSubscription;
      const ReadyToDefrostImprovedConfiguration_t *config;
      const DefrostData_t *defrostData;
   } _private;
} ReadyToDefrostImproved_t;

void ReadyToDefrostImproved_Init(
   ReadyToDefrostImproved_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostImprovedConfiguration_t *config,
   const DefrostData_t *defrostData);

#endif
