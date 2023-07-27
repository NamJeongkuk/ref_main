/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORLOADOFFDOOROPENPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORLOADOFFDOOROPENPLUGIN_H

#include "LoadOffDoorOpen.h"

typedef struct
{
   struct
   {
      LoadOffDoorOpen_t freshFoodLoadOffDoorOpen;
      LoadOffDoorOpen_t freezerLoadOffDoorOpen;
   } _private;
} SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_Init(
   SideBySideSingleEvaporatorLoadOffDoorOpenPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
