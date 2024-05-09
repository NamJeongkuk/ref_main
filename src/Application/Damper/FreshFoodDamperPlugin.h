/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FRESHFOODDAMPERPLUGIN_H
#define FRESHFOODDAMPERPLUGIN_H

#include "I_DataModel.h"
#include "DamperMotorPlugin.h"
#include "DamperRequestManager.h"
#include "DamperHeaterVotingFrameworkPlugin.h"
#include "DamperHeaterDefrostControl.h"
#include "PeriodicDamperCycling.h"
#include "DamperFreezePrevention.h"
#include "ErdResolver.h"
#include "SabbathDelayHandler.h"

typedef struct
{
   struct
   {
      ErdResolver_t damperPositionErdResolver;
      ErdResolver_t damperHeaterErdResolver;
      DamperMotorPlugin_t damperMotorPlugin;
      DamperRequestManager_t damperRequestManager;
      DamperHeaterVotingFrameworkPlugin_t damperHeaterVotingFrameworkPlugin;
      DamperFreezePrevention_t damperFreezePrevention;
      DamperHeaterDefrostControl_t damperHeaterDefrostControl;
      PeriodicDamperCycling_t periodicDamperCycling;
      SabbathDelayHandler_t sabbathDelayHandler;
   } _private;
} FreshFoodDamperPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void FreshFoodDamperPlugin_Init(FreshFoodDamperPlugin_t *instance, I_DataModel_t *dataModel);

#endif
