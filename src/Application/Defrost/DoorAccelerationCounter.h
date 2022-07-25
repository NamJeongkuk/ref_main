/*!
 * @file
 * @brief Counts the time that a door is open in which for each second its open
 * the door acceleration count increments by a parametric factor.
 * Door types include any fresh food door, freezer door, and convertible compartment door.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORACCELERATIONCOUNTER_H
#define DOORACCELERATIONCOUNTER_H

#include "I_DataModel.h"
#include "Fsm.h"
#include "DefrostData.h"
#include "GridData.h"
#include "Timer.h"

typedef struct
{
   Erd_t activelyWaitingForNextDefrostErd; // bool
   Erd_t doorAccelerationCounterFsmStateErd; // DefrostCompressorOnTimeCounterFsmState_t
   Erd_t freshFoodScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t freezerScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t convertibleCompartmentScaledDoorAccelerationInSecondsErd; // uint32_t
   Erd_t leftHandFreshFoodDoorIsOpenErd; // bool
   Erd_t rightHandFreshFoodDoorIsOpenErd; // bool
   Erd_t doorInDoorIsOpenErd; // bool
   Erd_t freezerDoorIsOpenErd; // bool
   Erd_t convertibleCompartmentDoorIsOpenErd; // bool
   Erd_t convertibleCompartmentStateErd; // ConvertibleCompartmentState_t
   Erd_t freezerFilteredTemperatureWasTooWarmOnPowerUpErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DoorAccelerationCounterConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t periodicTimer;
      EventSubscription_t subscription;
      const DoorAccelerationCounterConfiguration_t *config;
      const DefrostData_t *defrostParametricData;
   } _private;
} DoorAccelerationCounter_t;

void DoorAccelerationCounter_Init(
   DoorAccelerationCounter_t *instance,
   I_DataModel_t *dataModel,
   const DoorAccelerationCounterConfiguration_t *config);

#endif
