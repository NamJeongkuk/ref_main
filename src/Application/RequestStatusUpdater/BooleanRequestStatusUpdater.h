/*!
 * @file
 * @brief This module is used for the request / status bool erds.
 * The Status is always a boolean, but the request is a uint8_t.
 * The request is reset to a default value of 0xFF after it has been processed.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOOLEANREQUESTSTATUSUPDATER_H
#define BOOLEANREQUESTSTATUSUPDATER_H

#include "I_DataModel.h"

enum
{
   BooleanRequestStatus_ResetValue = 0xFF
};

typedef struct
{
   Erd_t requestErd; // uint8_t(acting as a bool)
   Erd_t statusErd; // bool
} BooleanRequestStatusErdPair_t;

typedef struct
{
   const BooleanRequestStatusErdPair_t *requestStatusErdPairs;
   uint8_t numberOfRequestStatusErdPairs;
} BooleanRequestStatusUpdaterConfig_t;

typedef struct
{
   struct
   {
      const BooleanRequestStatusUpdaterConfig_t *config;
      EventSubscription_t dataModelChangedSubscription;
      I_DataModel_t *dataModel;
   } _private;
} BooleanRequestStatusUpdater_t;

/*!
 * @param instance
 * @param config
 * @param dataModel
 */
void BooleanRequestStatusUpdater_Init(
   BooleanRequestStatusUpdater_t *instance,
   const BooleanRequestStatusUpdaterConfig_t *config,
   I_DataModel_t *dataModel);

#endif
