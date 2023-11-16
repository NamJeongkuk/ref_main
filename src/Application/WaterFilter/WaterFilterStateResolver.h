/*!
 * @file
 * @brief Writes to the water filter state based on a priority. The priority is as follows:
 * 1. Demo Mode
 * 2. Leak Detected
 * 3. Filter Error
 * 4. Bypass Plug Installed
 * 5. Water Filter Life Status (Expired/Replace/NoOrder)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERSTATERESOLVER_H
#define WATERFILTERSTATERESOLVER_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t enableDemoModeStatusErd; // bool
   Erd_t leakDetectedErd; // bool
   Erd_t filterErrorErd; // bool
   Erd_t bypassPlugInstalledErd; // bool
   Erd_t waterFilterLifeStatusErd; // WaterFilterLifeStatus_t
   Erd_t resolvedFilterStateErd; // WaterFilterState_t
} WaterFilterStateResolverConfig_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const WaterFilterStateResolverConfig_t *config;
      EventSubscription_t onDataModelChangeSubscription;
   } _private;
} WaterFilterStateResolver_t;

/*!
 * @param instance 
 * @param dataModel 
 * @param config 
 */
void WaterFilterStateResolver_Init(
   WaterFilterStateResolver_t *instance,
   I_DataModel_t *dataModel,
   const WaterFilterStateResolverConfig_t *config);

#endif
