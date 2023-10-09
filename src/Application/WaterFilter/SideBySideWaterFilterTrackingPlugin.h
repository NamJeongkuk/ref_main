/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEWATERFILTERTRACKINGPLUGIN_H
#define SIDEBYSIDEWATERFILTERTRACKINGPLUGIN_H

#include "WaterFilterLifeStatusUpdater.h"

typedef struct
{
   struct
   {
      WaterFilterLifeStatusUpdater_t waterFilterLifeStatusUpdater;
   } _private;
} SideBySideWaterFilterTrackingPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideWaterFilterTrackingPlugin_Init(
   SideBySideWaterFilterTrackingPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
