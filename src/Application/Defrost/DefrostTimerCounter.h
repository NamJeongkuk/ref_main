/*!
 * @file
 * @brief Counts time that compressor is on or in Sabbath mode and adds door accelerations
 * to the time when appropriate
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTIMERCOUNTER_H
#define DEFROSTTIMERCOUNTER_H

#include "I_DataModel.h"
#include "DefrostData.h"
#include "SabbathData.h"
#include "Timer.h"
#include "Fsm.h"

typedef struct
{
   Erd_t defrostTimerCounterFsmStateErd; // DefrostTimerCounterFsmState_t
   Erd_t defrostTimerCounterRequestErd; // DefrostTimerCounterRequest_t
   Erd_t defrostTimerCountInSecondsErd; // uint32_t
   Erd_t doorAccelerationRequestErd; // DoorAccelerationRequest_t
   Erd_t ffDoorAccelerationCountsErd; // uint16_t
   Erd_t fzDoorAccelerationCountsErd; // uint16_t
   Erd_t defrostTimerIsSatisfiedMonitorRequestErd; // DefrostTimerIsSatisfiedMonitorRequest_t
   Erd_t compressorResolvedSpeedErd; // CompressorVotedSpeed_t
   Erd_t compressorSpeedConfigErd; // CompressorSpeedConfig_t
   Erd_t sabbathModeErd; // bool
   Erd_t fzDefrostWasAbnormalErd; // bool
   Erd_t maxTimeBetweenDefrostsInMinutesErd; // uint16_t
   Erd_t timerModuleErd; // TimerModule_t *
} DefrostTimerCounterConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      Fsm_t fsm;
      Timer_t timer;
      const DefrostTimerCounterConfig_t *config;
      const DefrostData_t *defrostParametricData;
      const SabbathData_t *sabbathParametricData;
      EventSubscription_t requestSubscription;
   } _private;
} DefrostTimerCounter_t;

void DefrostTimerCounter_Init(
   DefrostTimerCounter_t *instance,
   I_DataModel_t *dataModel,
   const DefrostTimerCounterConfig_t *defrostTimerConfig);

#endif
