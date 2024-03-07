/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALLFREEZERDAMPERPLUGIN_H
#define ALLFREEZERDAMPERPLUGIN_H

#include "I_DataModel.h"
#include "DamperMotorPlugin.h"
#include "DamperRequestManager.h"
#include "DamperHeaterVotingFrameworkPlugin.h"
#include "DamperHeaterDefrostControl.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      ErdResolver_t damperPositionErdResolver;
      DamperMotorPlugin_t damperMotorPlugin;
      DamperRequestManager_t damperRequestManager;
   } _private;
} AllFreezerDamperPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void AllFreezerDamperPlugin_Init(AllFreezerDamperPlugin_t *instance, I_DataModel_t *dataModel);

#endif
