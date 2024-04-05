/*!
 * @file
 * @brief Implementation of Factory Mode
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FactoryMode.h"
#include "Constants_Binary.h"
#include "VoteType.h"
#include "StackAllocator.h"
#include "Signal.h"
#include "uassert.h"
#include <stdlib.h>
#include "Constants_Time.h"
#include "RampingPwmDutyCyclePercentageVote.h"

enum
{
   OneMinuteInMsec = 1 * MSEC_PER_MIN
};

typedef struct
{
   FactoryMode_t *instance;
   const uint8_t index;
   size_t erdSize;
} FactoryVoteErdCallbackContext_t;

static void FillOffValueInFactoryVote(void *context, void *factoryVoteData)
{
   FactoryVoteErdCallbackContext_t *factoryVoteErdCallbackContext = context;
   FactoryMode_t *instance = factoryVoteErdCallbackContext->instance;

   switch(factoryVoteErdCallbackContext->erdSize)
   {
      case sizeof(U8Vote_t): {
         U8Vote_t *u8Vote = factoryVoteData;
         u8Vote->value = instance->_private.config->factoryVoteList.pairs[factoryVoteErdCallbackContext->index].offValue;
         u8Vote->vote = Vote_Care;
      }
      break;

      case sizeof(U16Vote_t): {
         U16Vote_t *u16Vote = factoryVoteData;
         u16Vote->value = instance->_private.config->factoryVoteList.pairs[factoryVoteErdCallbackContext->index].offValue;
         u16Vote->vote = Vote_Care;
      }
      break;

      case sizeof(U32Vote_t): {
         U32Vote_t *u32Vote = factoryVoteData;
         u32Vote->value = instance->_private.config->factoryVoteList.pairs[factoryVoteErdCallbackContext->index].offValue;
         u32Vote->vote = Vote_Care;
      }
      break;

      default:
         uassert(!"Erd size not supported");
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->factoryVoteList.pairs[factoryVoteErdCallbackContext->index].erd,
      factoryVoteData);
}

static void FillOffValueInErdValuePairs(void *context, void *data)
{
   FactoryVoteErdCallbackContext_t *factoryVoteErdCallbackContext = context;
   FactoryMode_t *instance = factoryVoteErdCallbackContext->instance;

   switch(factoryVoteErdCallbackContext->erdSize)
   {
      case sizeof(uint8_t): {
         uint8_t *value = data;
         *value = instance->_private.config->erdValuePairList->pairs[factoryVoteErdCallbackContext->index].offValue;
      }
      break;

      case sizeof(uint16_t): {
         uint16_t *value = data;
         *value = instance->_private.config->erdValuePairList->pairs[factoryVoteErdCallbackContext->index].offValue;
      }
      break;

      case sizeof(uint32_t): {
         uint32_t *value = data;
         *value = instance->_private.config->erdValuePairList->pairs[factoryVoteErdCallbackContext->index].offValue;
      }
      break;

      default:
         uassert(!"Erd size not supported");
         break;
   }

   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->erdValuePairList->pairs[factoryVoteErdCallbackContext->index].erd,
      data);
}

static void FactoryModeOneMinuteTimerExpired(void *context)
{
   FactoryMode_t *instance = context;
   uint8_t factoryModeTime;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->factoryModeTimeErd,
      &factoryModeTime);

   factoryModeTime--;
   DataModel_Write(
      instance->_private.dataModel,
      instance->_private.config->factoryModeTimeErd,
      &factoryModeTime);
}

static void VoteOffForAllLights(FactoryMode_t *instance)
{
   RampingPwmDutyCyclePercentageVote_t lightVote;
   lightVote.rampingPwmDutyCyclePercentage.pwmDutyCyclePercentage = PercentageDutyCycle_Min;
   lightVote.rampingPwmDutyCyclePercentage.rampingDownCountInMsec = UINT8_MAX;
   lightVote.rampingPwmDutyCyclePercentage.rampingUpCountInMsec = UINT8_MAX;
   lightVote.care = Vote_Care;

   ErdList_WriteAll(
      instance->_private.dataModel,
      instance->_private.config->lightVoteErdList,
      &lightVote);
}

static void VoteOffForAllTheLoads(void *context)
{
   FactoryMode_t *instance = context;

   for(uint8_t erdIndex = 0; erdIndex < instance->_private.config->factoryVoteList.numberOfPairs; erdIndex++)
   {
      uint8_t erdSize = DataModel_SizeOf(
         instance->_private.dataModel,
         instance->_private.config->factoryVoteList.pairs[erdIndex].erd);

      FactoryVoteErdCallbackContext_t factoryVoteErdCallbackContext = {
         instance,
         erdIndex,
         erdSize
      };

      StackAllocator_Allocate(erdSize, FillOffValueInFactoryVote, &factoryVoteErdCallbackContext);
   }

   VoteOffForAllLights(instance);
}

static void WriteOffToErdValuePairs(void *context)
{
   FactoryMode_t *instance = context;

   for(uint8_t erdIndex = 0; erdIndex < instance->_private.config->erdValuePairList->numberOfPairs; erdIndex++)
   {
      uint8_t erdSize = DataModel_SizeOf(
         instance->_private.dataModel,
         instance->_private.config->erdValuePairList->pairs[erdIndex].erd);

      FactoryVoteErdCallbackContext_t factoryVoteErdCallbackContext = {
         instance,
         erdIndex,
         erdSize
      };

      StackAllocator_Allocate(erdSize, FillOffValueInErdValuePairs, &factoryVoteErdCallbackContext);
   }
}

static void FactoryModeTimeChanged(void *context, const void *args)
{
   FactoryMode_t *instance = context;
   const uint8_t *factoryModeEnableTimeInMinutes = args;

   if(*factoryModeEnableTimeInMinutes > 0)
   {
      if(instance->_private.factoryModeEntered == false)
      {
         if(instance->_private.config->erdValuePairList != NULL)
         {
            WriteOffToErdValuePairs(instance);
         }

         VoteOffForAllTheLoads(instance);
         instance->_private.factoryModeEntered = true;
      }

      TimerModule_StartOneShot(
         instance->_private.timerModule,
         &instance->_private.factoryModeOneMinuteTimer,
         OneMinuteInMsec,
         FactoryModeOneMinuteTimerExpired,
         instance);
   }
   else
   {
      Signal_SendViaErd(
         DataModel_AsDataSource(instance->_private.dataModel),
         instance->_private.config->broadcastResetRequestErd);
   }
}

void FactoryMode_Init(
   FactoryMode_t *instance,
   I_DataModel_t *dataModel,
   const FactoryModeConfiguration_t *factoryModeConfig,
   TimerModule_t *timerModule)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = factoryModeConfig;
   instance->_private.timerModule = timerModule;

   EventSubscription_Init(
      &instance->_private.factoryModeSubscription,
      instance,
      FactoryModeTimeChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->factoryModeTimeErd,
      &instance->_private.factoryModeSubscription);
}
