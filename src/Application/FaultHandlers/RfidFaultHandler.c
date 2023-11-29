/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "RfidFaultHandler.h"
#include "utils.h"

static void OnDataModelChange(void *context, const void *_args)
{
   RfidFaultHandler_t *instance = context;
   const RfidFaultRequest_t *faultRequestData = _args;

   for(uint8_t i = 0; i < instance->_private.config->numberOfFaults; i++)
   {
      if(instance->_private.config->faultConfigurations[i].faultErd == faultRequestData->faultErd)
      {
         if(faultRequestData->requestStatus)
         {
            instance->_private.faultRequestsCountBuffer[i] = TRUNCATE_UNSIGNED_ADDITION(instance->_private.faultRequestsCountBuffer[i], 1, UINT8_MAX);
            if(instance->_private.config->faultConfigurations[i].minimumConsecutiveRequestsToSetFault <= instance->_private.faultRequestsCountBuffer[i])
            {
               DataModel_Write(
                  instance->_private.dataModel,
                  instance->_private.config->faultConfigurations[i].faultErd,
                  set);
            }
         }
         else
         {
            instance->_private.faultRequestsCountBuffer[i] = 0;
            DataModel_Write(
               instance->_private.dataModel,
               instance->_private.config->faultConfigurations[i].faultErd,
               clear);
         }
         break;
      }
   }
}

void RfidFaultHandler_Init(
   RfidFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const RfidFaultHandlerConfig_t *config,
   uint8_t *faultRequestsCountBuffer)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;
   instance->_private.faultRequestsCountBuffer = faultRequestsCountBuffer;

   EventSubscription_Init(
      &instance->_private.onDataModelChangeSubscription,
      instance,
      OnDataModelChange);
   DataModel_Subscribe(
      dataModel,
      instance->_private.config->rfidFaultRequestErd,
      &instance->_private.onDataModelChangeSubscription);
}
