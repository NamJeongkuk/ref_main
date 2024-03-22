/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FreshFoodNonHeatedCycleDefrost.h"
#include "Constants_Binary.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "FanSpeed.h"
#include "SystemErds.h"
#include "utils.h"

enum
{
   Signal_CompressorStateChangedToOn = Hsm_UserSignalStart,
   Signal_CompressorStateChangedToOff,
   Signal_DefrostStateIsHeaterOn,
   Signal_TimerExpired,
   Signal_SealedSystemValveCurrentPositionChanged
};

static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_Running(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_CompressorIsOff(Hsm_t *hsm, HsmSignal_t signal, const void *data);
static bool State_CompressorIsOn(Hsm_t *hsm, HsmSignal_t signal, const void *data);

static const HsmStateHierarchyDescriptor_t stateList[] = {
   { State_Off, HSM_NO_PARENT },
   { State_Running, HSM_NO_PARENT },
   { State_CompressorIsOff, State_Running },
   { State_CompressorIsOn, State_Running }
};

static const HsmConfiguration_t hsmConfiguration = {
   stateList,
   NUM_ELEMENTS(stateList)
};

static FreshFoodNonHeatedCycleDefrost_t *InstanceFromHsm(Hsm_t *hsm)
{
   return CONTAINER_OF(FreshFoodNonHeatedCycleDefrost_t, _private.hsm, hsm);
}

static void VoteDontCareForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVote(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   FanVotedSpeed_t fanVotedSpeed = {
      .speed = FanSpeed_Off,
      .care = Vote_DontCare
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvapFanSpeedNonHeatedCycleDefrostVoteErd,
      &fanVotedSpeed);
}

static void VoteForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVote(FreshFoodNonHeatedCycleDefrost_t *instance, FanSpeed_t speed)
{
   FanVotedSpeed_t fanVotedSpeed = {
      .speed = speed,
      .care = Vote_Care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->freshFoodEvapFanSpeedNonHeatedCycleDefrostVoteErd,
      &fanVotedSpeed);
}

static DefrostState_t DefrostState(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   DefrostState_t state;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->defrostStateErd,
      &state);

   return state;
}

static TimerTicks_t CalculatedFreshFoodEvapFanRunningTimeInMsec(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   SetpointVotedTemperature_t freshFoodVotedSetpoint;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->freshFoodSetpointResolvedVoteErd,
      &freshFoodVotedSetpoint);

   uint16_t time = instance->_private.freshFoodNonHeatedCycleDefrostData->valveChangeSlopeInMinutesX10PerDegF;
   time = time * ((instance->_private.freshFoodUserSetpointRangeData->defaultTemperatureSetpoint * 100 - freshFoodVotedSetpoint.temperatureInDegFx100) / 100);
   time = time / 10;
   time = time + instance->_private.freshFoodNonHeatedCycleDefrostData->defaultTimeValveChangeInMinutes;
   time = CLAMP(
      time,
      instance->_private.freshFoodNonHeatedCycleDefrostData->minTimeValveChangeInMinutes,
      instance->_private.freshFoodNonHeatedCycleDefrostData->maxTimeValveChangeInMinutes);

   return time * MSEC_PER_MIN;
}

static uint8_t NumberOfEvaporators(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   return PersonalityParametricData_Get(instance->_private.dataModel)->platformData->numberOfEvaporators;
}

static SealedSystemValvePosition_t SealedSystemValvePreviousPosition(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   SealedSystemValvePosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValvePreviousPositionErd,
      &position);

   return position;
}

static SealedSystemValvePosition_t SealedSystemValveCurrentPosition(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   SealedSystemValvePosition_t position;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->sealedSystemValveCurrentPositionErd,
      &position);

   return position;
}

static bool CompressorIsOn(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   bool state;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->compressorIsOnErd,
      &state);

   return state;
}

static bool FanSpeedCompressorOffIsEnabled(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   return instance->_private.freshFoodNonHeatedCycleDefrostData->fanSpeedCompressorOffEnable;
}

static bool FanSpeedValveChangeIsEnabled(FreshFoodNonHeatedCycleDefrost_t *instance)
{
   return instance->_private.freshFoodNonHeatedCycleDefrostData->valveChangeEnable;
}

static bool State_Off(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   (void)data;
   FreshFoodNonHeatedCycleDefrost_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         VoteDontCareForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVote(instance);
         break;

      case Signal_CompressorStateChangedToOff:
         if((DefrostState(instance) != DefrostState_HeaterOn) && FanSpeedCompressorOffIsEnabled(instance))
         {
            switch(NumberOfEvaporators(instance))
            {
               case 1:
                  Hsm_Transition(hsm, State_CompressorIsOff);
                  break;

               case 2:
                  if(SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A ||
                     SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_C)
                  {
                     Hsm_Transition(hsm, State_CompressorIsOff);
                  }
                  break;

               case 3:
                  if(SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A ||
                     SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_D)
                  {
                     Hsm_Transition(hsm, State_CompressorIsOff);
                  }
                  break;
            }
         }
         break;

      case Signal_SealedSystemValveCurrentPositionChanged:
         if((DefrostState(instance) != DefrostState_HeaterOn) &&
            !CompressorIsOn(instance) &&
            FanSpeedCompressorOffIsEnabled(instance))
         {
            switch(NumberOfEvaporators(instance))
            {
               case 2:
                  if(SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A ||
                     SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_C)
                  {
                     Hsm_Transition(hsm, State_CompressorIsOff);
                  }
                  break;

               case 3:
                  if(SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A ||
                     SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_D)
                  {
                     Hsm_Transition(hsm, State_CompressorIsOff);
                  }
                  break;
            }
         }
         else if((DefrostState(instance) != DefrostState_HeaterOn) &&
            CompressorIsOn(instance) &&
            FanSpeedValveChangeIsEnabled(instance))
         {
            switch(NumberOfEvaporators(instance))
            {
               case 2:
                  if(((SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A) ||
                        (SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_C)) &&
                     SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_B)
                  {
                     Hsm_Transition(hsm, State_CompressorIsOn);
                  }
                  break;

               case 3:
                  if(((SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_A) ||
                        (SealedSystemValvePreviousPosition(instance) == SealedSystemValvePosition_D)) &&
                     ((SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_B) ||
                        (SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_C)))
                  {
                     Hsm_Transition(hsm, State_CompressorIsOn);
                  }
                  break;
            }
         }
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void TimeExpired(void *context)
{
   FreshFoodNonHeatedCycleDefrost_t *instance = context;
   Hsm_SendSignal(&instance->_private.hsm, Signal_TimerExpired, NULL);
}

static bool State_Running(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   (void)data;
   FreshFoodNonHeatedCycleDefrost_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         TimerModule_StartOneShot(
            DataModelErdPointerAccess_GetTimerModule(
               instance->_private.dataModel,
               Erd_TimerModule),
            &instance->_private.timer,
            CalculatedFreshFoodEvapFanRunningTimeInMsec(instance),
            TimeExpired,
            instance);
         break;

      case Signal_DefrostStateIsHeaterOn:
      case Signal_TimerExpired:
         Hsm_Transition(hsm, State_Off);
         break;

      case Signal_CompressorStateChangedToOn:
         switch(NumberOfEvaporators(instance))
         {
            case 1:
               Hsm_Transition(hsm, State_Off);
               break;

            case 2:
               if(SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_A ||
                  SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_C)
               {
                  Hsm_Transition(hsm, State_Off);
               }
               break;

            case 3:
               if(SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_A ||
                  SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_D)
               {
                  Hsm_Transition(hsm, State_Off);
               }
               break;
         }
         break;

      case Signal_SealedSystemValveCurrentPositionChanged:
         if(CompressorIsOn(instance))
         {
            switch(NumberOfEvaporators(instance))
            {
               case 2:
                  if((SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_A) ||
                     (SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_C))
                  {
                     Hsm_Transition(hsm, State_Off);
                  }
                  break;

               case 3:
                  if((SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_A) ||
                     (SealedSystemValveCurrentPosition(instance) == SealedSystemValvePosition_D))
                  {
                     Hsm_Transition(hsm, State_Off);
                  }
                  break;
            }
         }
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_CompressorIsOff(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   (void)data;
   FreshFoodNonHeatedCycleDefrost_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         VoteForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVote(
            instance,
            instance->_private.freshFoodNonHeatedCycleDefrostData->fanSpeedCompressorOff);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static bool State_CompressorIsOn(Hsm_t *hsm, HsmSignal_t signal, const void *data)
{
   (void)data;
   FreshFoodNonHeatedCycleDefrost_t *instance = InstanceFromHsm(hsm);

   switch(signal)
   {
      case Hsm_Entry:
         VoteForFreshFoodEvapFanSpeedNonHeatedCycleDefrostVote(
            instance,
            instance->_private.freshFoodNonHeatedCycleDefrostData->fanSpeedValveChange);
         break;

      default:
         return HsmSignalDeferred;
   }

   return HsmSignalConsumed;
}

static void DataModelUpdated(void *context, const void *_args)
{
   FreshFoodNonHeatedCycleDefrost_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->compressorIsOnErd)
   {
      const bool *compressorIsOn = args->data;
      if(*compressorIsOn)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateChangedToOn, NULL);
      }
      else
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_CompressorStateChangedToOff, NULL);
      }
   }
   else if(args->erd == instance->_private.config->defrostStateErd)
   {
      const DefrostState_t *state = args->data;
      if(*state == DefrostState_HeaterOn)
      {
         Hsm_SendSignal(&instance->_private.hsm, Signal_DefrostStateIsHeaterOn, NULL);
      }
   }
   else if(args->erd == instance->_private.config->sealedSystemValveCurrentPositionErd)
   {
      Hsm_SendSignal(&instance->_private.hsm, Signal_SealedSystemValveCurrentPositionChanged, NULL);
   }
}

void FreshFoodNonHeatedCycleDefrost_Init(
   FreshFoodNonHeatedCycleDefrost_t *instance,
   I_DataModel_t *dataModel,
   const FreshFoodNonHeatedCycleDefrostConfig_t *config,
   const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrostData,
   const UserSetpointRangeData_t *freshFoodUserSetpointRangeData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.freshFoodNonHeatedCycleDefrostData = freshFoodNonHeatedCycleDefrostData;
   instance->_private.freshFoodUserSetpointRangeData = freshFoodUserSetpointRangeData;

   EventSubscription_Init(
      &instance->_private.dataModelSubscription,
      instance,
      DataModelUpdated);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.dataModelSubscription);

   Hsm_Init(&instance->_private.hsm, &hsmConfiguration, State_Off);
}
