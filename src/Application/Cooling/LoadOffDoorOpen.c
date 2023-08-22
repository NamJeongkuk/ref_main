/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "LoadOffDoorOpen.h"
#include "Vote.h"
#include "VoteType.h"
#include "StackAllocator.h"
#include "Constants_Time.h"
#include "DataModelErdPointerAccess.h"
#include "uassert.h"

typedef struct
{
   LoadOffDoorOpen_t *instance;
   const uint8_t index;
   size_t erdSize;
} LoadVoteErdCallbackContext_t;

enum
{
   Signal_AnAssociatedDoorIsOpened = Fsm_UserSignalStart,
   Signal_AllAssociatedDoorsAreClosed,
   Signal_TimeoutExpired
};

static void State_AnAssociatedDoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_AllAssociatedDoorsAreClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static LoadOffDoorOpen_t *InstanceFromFsm(Fsm_t *fsm)
{
   return CONTAINER_OF(LoadOffDoorOpen_t, _private.fsm, fsm);
}

static void FillDontCareInLoadVote(void *context, void *loadVoteData)
{
   LoadVoteErdCallbackContext_t *loadVoteErdCallbackContext = context;
   LoadOffDoorOpen_t *instance = loadVoteErdCallbackContext->instance;

   switch(loadVoteErdCallbackContext->erdSize)
   {
      case sizeof(U8Vote_t): {
         U8Vote_t *u8Vote = loadVoteData;
         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].loadVoteErd,
            u8Vote);
         u8Vote->vote = Vote_DontCare;
      }
      break;

      case sizeof(U16Vote_t): {
         U16Vote_t *u16Vote = loadVoteData;
         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].loadVoteErd,
            u16Vote);
         u16Vote->vote = Vote_DontCare;
      }
      break;

      case sizeof(U32Vote_t): {
         U32Vote_t *u32Vote = loadVoteData;
         DataModel_Read(
            instance->_private.dataModel,
            instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].loadVoteErd,
            u32Vote);
         u32Vote->vote = Vote_DontCare;
      }
      break;

      default:
         uassert(!"Erd size not supported");
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].loadVoteErd,
      loadVoteData);
}

static void FillOffValueInLoadVote(void *context, void *loadVoteData)
{
   LoadVoteErdCallbackContext_t *loadVoteErdCallbackContext = context;
   LoadOffDoorOpen_t *instance = loadVoteErdCallbackContext->instance;

   switch(loadVoteErdCallbackContext->erdSize)
   {
      case sizeof(U8Vote_t): {
         U8Vote_t *u8Vote = loadVoteData;
         u8Vote->value = instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].offValue;
         u8Vote->vote = Vote_Care;
      }
      break;

      case sizeof(U16Vote_t): {
         U16Vote_t *u16Vote = loadVoteData;
         u16Vote->value = instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].offValue;
         u16Vote->vote = Vote_Care;
      }
      break;

      case sizeof(U32Vote_t): {
         U32Vote_t *u32Vote = loadVoteData;
         u32Vote->value = instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].offValue;
         u32Vote->vote = Vote_Care;
      }
      break;

      default:
         uassert(!"Erd size not supported");
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->compartmentVoteErdList.pairs[loadVoteErdCallbackContext->index].loadVoteErd,
      loadVoteData);
}

static void VoteOffAllLoads(LoadOffDoorOpen_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->compartmentVoteErdList.numberOfPairs; i++)
   {
      uint8_t erdSize = DataModel_SizeOf(
         instance->_private.dataModel,
         instance->_private.config->compartmentVoteErdList.pairs[i].loadVoteErd);

      LoadVoteErdCallbackContext_t loadVoteErdCallbackContext = {
         instance,
         i,
         erdSize
      };

      StackAllocator_Allocate(i, FillOffValueInLoadVote, &loadVoteErdCallbackContext);
   }
}

static void VoteDontCareAllLoads(LoadOffDoorOpen_t *instance)
{
   for(uint8_t i = 0; i < instance->_private.config->compartmentVoteErdList.numberOfPairs; i++)
   {
      uint8_t erdSize = DataModel_SizeOf(
         instance->_private.dataModel,
         instance->_private.config->compartmentVoteErdList.pairs[i].loadVoteErd);

      LoadVoteErdCallbackContext_t loadVoteErdCallbackContext = {
         instance,
         i,
         erdSize
      };

      StackAllocator_Allocate(i, FillDontCareInLoadVote, &loadVoteErdCallbackContext);
   }
}

static void TimeoutExpires(void *context)
{
   LoadOffDoorOpen_t *instance = context;
   Fsm_SendSignal(&instance->_private.fsm, Signal_TimeoutExpired, NULL);
}

static bool DoorIsAssociated(LoadOffDoorOpen_t *instance, uint8_t doorsBitmapIndex)
{
   return BITMAP_STATE(instance->_private.data->doorsBitmap.bitmap, doorsBitmapIndex);
}

static bool AnAssociatedDoorChanged(LoadOffDoorOpen_t *instance, Erd_t erd)
{
   for(uint8_t i = 0; i < (sizeof(instance->_private.config->doorStatus) / sizeof(Erd_t)); i++)
   {
      Erd_t doorStatusErd = ((const Erd_t *)&instance->_private.config->doorStatus)[i];
      if(doorStatusErd != Erd_Invalid)
      {
         if(DoorIsAssociated(instance, i) &&
            erd == doorStatusErd)
         {
            return true;
         }
      }
   }

   return false;
}

static bool AnAssociatedDoorIsOpen(LoadOffDoorOpen_t *instance)
{
   for(uint8_t i = 0; i < (sizeof(instance->_private.config->doorStatus) / sizeof(Erd_t)); i++)
   {
      Erd_t doorStatusErd = ((const Erd_t *)&instance->_private.config->doorStatus)[i];
      if(doorStatusErd != Erd_Invalid)
      {
         bool doorIsOpen;
         DataModel_Read(
            instance->_private.dataModel,
            doorStatusErd,
            &doorIsOpen);

         if(DoorIsAssociated(instance, i) && doorIsOpen)
         {
            return true;
         }
      }
   }

   return false;
}

static bool AllAssociatedDoorsAreClosed(LoadOffDoorOpen_t *instance)
{
   return !(AnAssociatedDoorIsOpen(instance));
}

static void StopTimeoutTimer(LoadOffDoorOpen_t *instance)
{
   TimerModule_Stop(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timeoutTimer);
}

static void StartTimeoutTimerFor(LoadOffDoorOpen_t *instance, TimerTicks_t ticks)
{
   TimerModule_StartOneShot(
      DataModelErdPointerAccess_GetTimerModule(
         instance->_private.dataModel,
         instance->_private.config->timerModuleErd),
      &instance->_private.timeoutTimer,
      ticks,
      TimeoutExpires,
      instance);
}

void State_AnAssociatedDoorIsOpen(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   LoadOffDoorOpen_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         VoteOffAllLoads(instance);
         if(instance->_private.data->timeoutInSeconds > 0)
         {
            StartTimeoutTimerFor(
               instance,
               instance->_private.data->timeoutInSeconds * MSEC_PER_SEC);
         }
         break;

      case Signal_AnAssociatedDoorIsOpened:
         // purposefully ignored
         break;

      case Signal_AllAssociatedDoorsAreClosed:
         Fsm_Transition(&instance->_private.fsm, State_AllAssociatedDoorsAreClosed);
         break;

      case Signal_TimeoutExpired:
         VoteDontCareAllLoads(instance);
         break;

      case Fsm_Exit:
         StopTimeoutTimer(instance);
         break;
   }
}

void State_AllAssociatedDoorsAreClosed(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   LoadOffDoorOpen_t *instance = InstanceFromFsm(fsm);
   IGNORE(data);

   switch(signal)
   {
      case Fsm_Entry:
         StartTimeoutTimerFor(
            instance,
            instance->_private.data->restartDelayInSeconds * MSEC_PER_SEC);
         break;

      case Signal_AnAssociatedDoorIsOpened:
         Fsm_Transition(&instance->_private.fsm, State_AnAssociatedDoorIsOpen);
         break;

      case Signal_TimeoutExpired:
         VoteDontCareAllLoads(instance);
         break;

      case Fsm_Exit:
         StopTimeoutTimer(instance);
         break;
   }
}

static void OnDataModelChange(void *context, const void *_args)
{
   LoadOffDoorOpen_t *instance = context;
   const DataModelOnDataChangeArgs_t *args = _args;

   if(AnAssociatedDoorChanged(instance, args->erd))
   {
      if(AllAssociatedDoorsAreClosed(instance))
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_AllAssociatedDoorsAreClosed, NULL);
      }
      else
      {
         Fsm_SendSignal(&instance->_private.fsm, Signal_AnAssociatedDoorIsOpened, NULL);
      }
   }
}

static FsmState_t InitialState(LoadOffDoorOpen_t *instance)
{
   if(AnAssociatedDoorIsOpen(instance))
   {
      return State_AnAssociatedDoorIsOpen;
   }

   return State_AllAssociatedDoorsAreClosed;
}

/*!
 * @param instance
 * @param dataModel
 * @param data
 * @param config
 */
void LoadOffDoorOpen_Init(
   LoadOffDoorOpen_t *instance,
   I_DataModel_t *dataModel,
   const LoadOffDoorOpenCompartmentData_t *loadOffDoorOpenCompartmentData,
   const LoadOffDoorOpenConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.data = loadOffDoorOpenCompartmentData;

   Fsm_Init(
      &instance->_private.fsm,
      InitialState(instance));

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_SubscribeAll(
      instance->_private.dataModel,
      &instance->_private.onDataModelChangeSubscription);
}
