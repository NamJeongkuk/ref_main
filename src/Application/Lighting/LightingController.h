/*!
 * @file
 * @brief Lighting controller which copies the values from an inputted voted PWM duty cycle into a mapped BSP PWM duty cycle
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef LIGHTINGCONTROLLER_H
#define LIGHTINGCONTROLLER_H

#include "I_DataModel.h"
#include "I_ConstArrayMap.h"
#include "LightingData.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t onDataModelChangeSubscription;
      I_ConstArrayMap_t *votedPwmToPwmMap;
      const LightingData_t *lightingData;
   } _private;
} LightingController_t;

/*!
 * @param instance
 * @param dataModel
 * @param votedPwmToPwmMap
 */
void LightingController_Init(
   LightingController_t *instance,
   I_DataModel_t *dataModel,
   I_ConstArrayMap_t *votedPwmToPwmMap,
   const LightingData_t *lightingData);

#endif
