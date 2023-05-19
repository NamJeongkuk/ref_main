/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "IceMakerMoldHeaterController.h"
#include "IceMakerMoldHeaterControlRequest.h"
#include "HeaterVotedState.h"
#include "TemperatureDegFx100.h"
#include "DataModelErdPointerAccess.h"
#include "Constants_Time.h"
#include "utils.h"

#define InstanceFromFsm(fsm) CONTAINER_OF(IceMakerMoldHeaterController_t, _private.fsm, fsm)

enum
{
   Signal_Disable = Fsm_UserSignalStart,
   Signal_Enable,
   Signal_IceMakerTemperatureChanged,
   Signal_HeaterTimerExpired,
   Signal_IceMakerTemperatureGreaterThanMotorOnTemperature,
};

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_InitialMoldHeaterOn(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_MoldHeaterOff(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_MoldHeaterOn(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static void SetMoldHeaterVoteTo(IceMakerMoldHeaterController_t *instance, bool state, bool care)
{
   HeaterVotedState_t votedState = {
      .state = state,
      .care = care
   };

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterVoteErd,
      &votedState);
}

static TemperatureDegFx100_t IceMakerMoldTemperature(IceMakerMoldHeaterController_t *instance)
{
   TemperatureDegFx100_t currentTemperature;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldFilteredTemperatureInDegFx100Erd,
      &currentTemperature);

   return currentTemperature;
}

static IceMakerMoldHeaterControlRequest_t IceMakerHeaterRequest(IceMakerMoldHeaterController_t *instance)
{
   IceMakerMoldHeaterControlRequest_t request;
   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->moldHeaterControlRequestErd,
      &request);

   return request;
}

static void HeaterTimerExpired(void *context)
{
   IceMakerMoldHeaterController_t *instance = context;

   Fsm_SendSignal(&instance->_private.fsm, Signal_HeaterTimerExpired, NULL);
}

static void StartHeaterTimer(IceMakerMoldHeaterController_t *instance)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.heaterTimer,
      instance->_private.harvestData->initialMinimumHeaterOnTimeInSeconds * MSEC_PER_SEC,
      HeaterTimerExpired,
      instance);
}

static void StopHeaterTimer(IceMakerMoldHeaterController_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(instance->_private.dataModel, instance->_private.config->timerModuleErd),
      &instance->_private.heaterTimer);
}

static void CompareIceMakerMoldTemperatureToInitialMotorOnTemperature(IceMakerMoldHeaterController_t *instance)
{
   if(IceMakerMoldTemperature(instance) > instance->_private.harvestData->initialMotorOnTemperatureInDegFx100)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerTemperatureGreaterThanMotorOnTemperature, NULL);
   }
}

static void State_Disabled(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IceMakerMoldHeaterController_t *instance = InstanceFromFsm(fsm);

   switch(signal)
   {
      case Fsm_Entry:
         SetMoldHeaterVoteTo(instance, HeaterState_Off, Vote_DontCare);
         break;

      case Signal_Enable: {
         const bool *skipInitialTime = data;
         if(*skipInitialTime)
         {
            if(IceMakerMoldTemperature(instance) < IceMakerHeaterRequest(instance).offTemperatureInDegFx100)
            {
               Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOn);
            }
            else
            {
               Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOff);
            }
         }
         else
         {
            Fsm_Transition(&instance->_private.fsm, State_InitialMoldHeaterOn);
         }
      }
      break;

      case Fsm_Exit:
         break;
   }
}

static void State_InitialMoldHeaterOn(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IceMakerMoldHeaterController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetMoldHeaterVoteTo(instance, HeaterState_On, Vote_Care);
         StartHeaterTimer(instance);
         CompareIceMakerMoldTemperatureToInitialMotorOnTemperature(instance);
         break;

      case Signal_Disable:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Signal_IceMakerTemperatureGreaterThanMotorOnTemperature:
      case Signal_HeaterTimerExpired: {
         if(IceMakerMoldTemperature(instance) < IceMakerHeaterRequest(instance).offTemperatureInDegFx100)
         {
            Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOn);
         }
         else
         {
            Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOff);
         }
      }
      break;

      case Fsm_Exit:
         StopHeaterTimer(instance);
         break;
   }
}

static void State_MoldHeaterOn(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IceMakerMoldHeaterController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetMoldHeaterVoteTo(instance, HeaterState_On, Vote_Care);
         break;

      case Signal_IceMakerTemperatureChanged: {
         if(IceMakerMoldTemperature(instance) >= IceMakerHeaterRequest(instance).offTemperatureInDegFx100)
         {
            Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOff);
         }
      }
      break;

      case Signal_Disable:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Fsm_Exit:
         break;
   }
}

static void State_MoldHeaterOff(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IceMakerMoldHeaterController_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         SetMoldHeaterVoteTo(instance, HeaterState_Off, Vote_Care);
         break;

      case Signal_IceMakerTemperatureChanged: {
         if(IceMakerMoldTemperature(instance) <= IceMakerHeaterRequest(instance).onTemperatureInDegFx100)
         {
            Fsm_Transition(&instance->_private.fsm, State_MoldHeaterOn);
         }
      }
      break;

      case Signal_Disable:
         Fsm_Transition(&instance->_private.fsm, State_Disabled);
         break;

      case Fsm_Exit:
         break;
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   IceMakerMoldHeaterController_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(args->erd == instance->_private.config->moldHeaterControlRequestErd)
   {
      const IceMakerMoldHeaterControlRequest_t *request = args->data;
      if(request->enable)
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_Enable, &request->skipInitialOnTime);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_Disable, NULL);
      }
   }
   else if(args->erd == instance->_private.config->moldFilteredTemperatureInDegFx100Erd)
   {
      Fsm_SendSignal(&instance->_private.fsm, Signal_IceMakerTemperatureChanged, NULL);
   }
}

void IceMakerMoldHeaterController_Init(
   IceMakerMoldHeaterController_t *instance,
   I_DataModel_t *dataModel,
   const IceMakerMoldHeaterControllerConfig_t *config,
   const AluminumMoldIceMakerHarvestData_t *harvestData)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.harvestData = harvestData;

   Fsm_Init(&instance->_private.fsm, State_Disabled);

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
