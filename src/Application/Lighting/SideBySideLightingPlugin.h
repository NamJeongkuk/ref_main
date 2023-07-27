/*!
 * @file
 * @brief Lighting plugin specifically for side by side models.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDELIGHTINGPLUGIN_H
#define SIDEBYSIDELIGHTINGPLUGIN_H

#include "I_DataModel.h"
#include "LightingController.h"
#include "ErdResolver.h"
#include "ConstArrayMap_LinearSearch.h"
#include "LightingDoorVoteResolver.h"

typedef struct
{
   struct
   {
      LightingController_t lightingController;
      ErdResolver_t freshFoodBackWallLightErdResolver;
      ErdResolver_t freshFoodTopLightErdResolver;
      ErdResolver_t freezerBackWallLightErdResolver;
      ErdResolver_t freezerTopLightErdResolver;
      ConstArrayMap_LinearSearch_t linearMap;
      LightingDoorVoteResolver_t freshFoodBackWallLightingDoorVoteResolver;
      LightingDoorVoteResolver_t freshFoodTopLightLightingDoorVoteResolver;
      LightingDoorVoteResolver_t freezerBackWallLightingDoorVoteResolver;
      LightingDoorVoteResolver_t freezerTopLightLightingDoorVoteResolver;
   } _private;
} SideBySideLightingPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideLightingPlugin_Init(
   SideBySideLightingPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
