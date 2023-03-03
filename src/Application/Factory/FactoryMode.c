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
#include "uassert.h"
#include <stdlib.h>

enum
{
   ResetDelayTimeInSeconds = 1
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
      instance->_private.config->factoryVoteList.pairs[factoryVoteErdCallbackContext->index].factoryVoteErd,
      factoryVoteData);
}

static void FactoryModeActiveChanged(void *context, const void *args)
{
   FactoryMode_t *instance = context;
   const bool *factoryModeIsActive = args;
   uint8_t delay = ResetDelayTimeInSeconds;

   if(*factoryModeIsActive)
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->disableMinimumCompressorTimesErd,
         set);

      for(uint8_t erdIndex = 0; erdIndex < instance->_private.config->factoryVoteList.numberOfPairs; erdIndex++)
      {
         uint8_t erdSize = DataModel_SizeOf(
            instance->_private.dataModel,
            instance->_private.config->factoryVoteList.pairs[erdIndex].factoryVoteErd);

         FactoryVoteErdCallbackContext_t factoryVoteErdCallbackContext = {
            instance,
            erdIndex,
            erdSize
         };

         StackAllocator_Allocate(erdSize, FillOffValueInFactoryVote, &factoryVoteErdCallbackContext);
      }
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->resetErd,
         &delay);
   }
}

void FactoryMode_Init(
   FactoryMode_t *instance,
   I_DataModel_t *dataModel,
   const FactoryModeConfiguration_t *factoryModeConfig)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = factoryModeConfig;

   EventSubscription_Init(
      &instance->_private.factoryModeSubscription,
      instance,
      FactoryModeActiveChanged);
   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->factoryModeActiveErd,
      &instance->_private.factoryModeSubscription);
}
