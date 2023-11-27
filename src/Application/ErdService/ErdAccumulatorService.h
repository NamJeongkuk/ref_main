/*!
 * @file
 * @brief This module sums up an input ERD's on-change values and writes the result to an ouput ERD
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ERDACCUMULATORSERVICE_H
#define ERDACCUMULATORSERVICE_H

#include "I_DataModel.h"
#include "SystemErds.h"

typedef void (*AccumulatorServiceFunction_t)(void *context, const void *data);

typedef struct
{
   ErdList_t inputErdList;
   Erd_t cumulativeValueErd;
} ErdAccumulatorServiceConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const ErdAccumulatorServiceConfig_t *config;
      EventSubscription_t dataModelChangedSubscription;
      AccumulatorServiceFunction_t accumulatorServiceFunction;
   } _private;
} ErdAccumulatorService_t;

/*!
 * @param instance
 * @param dataModel
 * @param config
 * @param integerWidth the size of the datatype
 * @param integerIsSigned a boolean value indicating if the datatype is signed or not
 */
void ErdAccumulatorService_Init(
   ErdAccumulatorService_t *instance,
   I_DataModel_t *dataModel,
   const ErdAccumulatorServiceConfig_t *config,
   uint8_t integerWidth,
   bool integerIsSigned);

#endif
