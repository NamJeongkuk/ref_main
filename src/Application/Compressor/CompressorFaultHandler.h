/*!
 * @file
 * @brief This module will set compressor related faults
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COMPRESSORFAULTHANDLER_H
#define COMPRESSORFAULTHANDLER_H

#include "I_DataModel.h"
#include "Timer.h"
#include "CompressorTimes.h"

typedef struct
{
   Erd_t compressorIsOnErd; // bool
   Erd_t coolingOffStatusErd; // bool
   Erd_t excessiveCompressorRunTimeFaultErd; // bool
   Erd_t compressorOffWhileCoolingSystemOnForMaxTimeFaultErd; // bool
   Erd_t timerModuleErd; // TimerModule_t *
} CompressorFaultHandlerConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const CompressorFaultHandlerConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
      bool initialCycleComplete;
      const CompressorTimes_t *compressorTimes;
      Timer_t timer;
   } _private;
} CompressorFaultHandler_t;

/*!
 * @param instance 
 * @param dataModel 
 * @param config 
 * @param compressorTimes 
 */
void CompressorFaultHandler_Init(
   CompressorFaultHandler_t *instance,
   I_DataModel_t *dataModel,
   const CompressorFaultHandlerConfig_t *config,
   const CompressorTimes_t *compressorTimes);

#endif
