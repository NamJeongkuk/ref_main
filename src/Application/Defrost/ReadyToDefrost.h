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

#ifndef READYTODEFROST_H
#define READYTODEFROST_H

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
   Erd_t defrostPowerUpReadyErd; // bool
   Erd_t cabinetFilteredTemperatureTooWarmOnPowerUpErd; // bool
   Erd_t useAhamPrechillReadyToDefrostTimeAndResetDefrostCountsErd; // bool
   Erd_t invalidEvaporatorThermistorDuringDefrostErd; // bool
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
} ReadyToDefrostConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Hsm_t hsm;
      Timer_t timer;
      EventSubscription_t dataModelSubscription;
      const ReadyToDefrostConfiguration_t *config;
      const DefrostData_t *defrostData;
   } _private;
} ReadyToDefrost_t;

void ReadyToDefrost_Init(
   ReadyToDefrost_t *instance,
   I_DataModel_t *dataModel,
   const ReadyToDefrostConfiguration_t *config,
   const DefrostData_t *defrostData);

#endif
