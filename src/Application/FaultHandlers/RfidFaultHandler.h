/*!
 * @file
 * @brief Fault Handler for Rfid faults.
 * Will set requested faults once their fault count has been reached
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDFAULTHANDLER_H
#define RFIDFAULTHANDLER_H

#include "I_DataModel.h"
#include "Signal.h"

typedef struct
{
   Erd_t faultErd;
   bool requestStatus;
   Signal_t signal;
} RfidFaultRequest_t;

#define SWAP_DEFINITION_RfidFaultRequest_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, faultErd),

typedef struct
{
   Erd_t faultErd;
   uint8_t minimumConsecutiveRequestsToSetFault;
} RfidFaultConfig_t;

typedef struct
{
   const RfidFaultConfig_t *faultConfigurations;
   uint8_t numberOfFaults;
   Erd_t rfidFaultRequestErd; // RfidFaultRequest_t
} RfidFaultHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onDataModelChangeSubscription;
      const RfidFaultHandlerConfig_t *config;
      uint8_t *faultRequestsCountBuffer;
   } _private;
} RfidFaultHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param faultRequestsCountBuffer Buffer to hold the current counts of the fault requests
 */
void RfidFaultHandler_Init(
   RfidFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const RfidFaultHandlerConfig_t *config,
   uint8_t *faultRequestsCountBuffer);

#endif
