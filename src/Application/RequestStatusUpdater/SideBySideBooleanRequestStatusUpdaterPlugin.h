/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEBOOLEANREQUESTSTATUSUPDATERPLUGIN_H
#define SIDEBYSIDEBOOLEANREQUESTSTATUSUPDATERPLUGIN_H

#include "BooleanRequestStatusUpdater.h"

typedef struct
{
   struct
   {
      BooleanRequestStatusUpdater_t booleanRequestStatusUpdater;
   } _private;
} SideBySideBooleanRequestStatusUpdaterPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideBooleanRequestStatusUpdaterPlugin_Init(
   SideBySideBooleanRequestStatusUpdaterPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
