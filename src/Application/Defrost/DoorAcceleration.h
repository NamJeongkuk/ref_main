/*!
 * @file
 * @brief Monitors door openings and accumulates door acceleration time
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORACCELERATION_H
#define DOORACCELERATION_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "Fsm.h"
#include "Timer.h"

typedef struct
{
   Erd_t doorAccelerationRequestErd; // DoorAccelerationRequest_t
   Erd_t doorAccelerationFsmStateErd; // DoorAccelerationFsmState_t
   Erd_t fzDoorAccelerationCountsErd; // uint32_t
   Erd_t ffDoorAccelerationCountsErd; // uint32_t
   Erd_t leftHandFfDoorIsOpenErd; // bool
   Erd_t rightHandFfDoorIsOpenErd; // bool
   Erd_t doorInDoorIsOpenErd; // bool
   Erd_t fzDoorIsOpenErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} DoorAccelerationConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t timer;
      const DoorAccelerationConfig_t *config;
      const DefrostData_t *defrostParametricData;
      EventSubscription_t dataModelChangedSubscription;
   } _private;
} DoorAccelerationCalculator_t;

void DoorAcceleration_Init(
   DoorAccelerationCalculator_t *instance,
   I_DataModel_t *dataModel,
   const DoorAccelerationConfig_t *doorAccelerationConfig);

#endif
