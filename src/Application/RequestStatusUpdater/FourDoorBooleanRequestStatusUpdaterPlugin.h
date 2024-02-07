/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORBOOLEANREQUESTSTATUSUPDATERPLUGIN_H
#define FOURDOORBOOLEANREQUESTSTATUSUPDATERPLUGIN_H

#include "BooleanRequestStatusUpdater.h"

typedef struct
{
   struct
   {
      BooleanRequestStatusUpdater_t booleanRequestStatusUpdater;
   } _private;
} FourDoorBooleanRequestStatusUpdaterPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FourDoorBooleanRequestStatusUpdaterPlugin_Init(
   FourDoorBooleanRequestStatusUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
