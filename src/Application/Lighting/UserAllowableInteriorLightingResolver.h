/*!
 * @file
 * @brief For each light, it updates the user allowable state ERD with
 * whether or not it's user allowable (based on all user allowable lighting
 * config ERDs) and if it is user allowable, it multiplies that light's
 * user request status by parametric's normal operation PWM duty cycle
 * for that light.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERALLOWABLEINTERIORLIGHTINGRESOLVER_H
#define USERALLOWABLEINTERIORLIGHTINGRESOLVER_H

#include "I_DataModel.h"
#include "LightingData.h"

typedef struct
{
   Erd_t statusErd; // PercentageDutyCycle_t
   Erd_t configErd; // UserAllowableInteriorLightingBitmap_t
} UserAllowableLightConfig_t;

typedef struct
{
   struct
   {
      Erd_t freshFoodBackLightUserAllowableStateErd; // UserAllowableInteriorLightState_t
      Erd_t freshFoodTopLightUserAllowableStateErd; // UserAllowableInteriorLightState_t
      Erd_t freezerBackLightUserAllowableStateErd; // UserAllowableInteriorLightState_t
      Erd_t freezerTopLightUserAllowableStateErd; // UserAllowableInteriorLightState_t
   } lights;

   const UserAllowableLightConfig_t *userAllowableLightConfigs;
   uint8_t numberOfUserAllowableLightConfigs;
} UserAllowableInteriorLightingResolverConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const UserAllowableInteriorLightingResolverConfiguration_t *config;
      const LightingData_t *lightingData;
   } _private;
} UserAllowableInteriorLightingResolver_t;

void UserAllowableInteriorLightingResolver_Init(
   UserAllowableInteriorLightingResolver_t *instance,
   I_DataModel_t *dataModel,
   const LightingData_t *lightingData,
   const UserAllowableInteriorLightingResolverConfiguration_t *config);

#endif
