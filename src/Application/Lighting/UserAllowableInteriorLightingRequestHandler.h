/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef USERALLOWABLEINTERIORLIGHTINGREQUESTHANDLER_H
#define USERALLOWABLEINTERIORLIGHTINGREQUESTHANDLER_H

#include "I_DataModel.h"
#include "EventSubscription.h"
#include "LightingData.h"

typedef struct
{
   Erd_t userAllowableInteriorLightingRequestErd; // PercentageDutyCycle_t
   Erd_t userAllowableInteriorLightingStateErd; // PercentageDutyCycle_t
   Erd_t userAllowableInteriorLightingConfigurationErd; // UserAllowableInteriorLightingBitmap_t
} UserAllowableLightingConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelSubscription;
      const UserAllowableLightingConfiguration_t *config;
   } _private;
} UserAllowableInteriorLightingRequestHandler_t;

void UserAllowableInteriorLightingRequestHandler_Init(
   UserAllowableInteriorLightingRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   const UserAllowableInteriorLightingBitmap_t lightingBitmap,
   const UserAllowableLightingConfiguration_t *config);

#endif
