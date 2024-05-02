/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTIBLECOMPARTMENTDAMPERPLUGIN_H
#define CONVERTIBLECOMPARTMENTDAMPERPLUGIN_H

#include "I_DataModel.h"
#include "DamperMotorPlugin.h"
#include "DamperRequestManager.h"
#include "DamperHeaterVotingFrameworkPlugin.h"
#include "DamperHeaterDefrostControl.h"
#include "DamperMaxOpenTimeMonitor.h"
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
      DamperMaxOpenTime_t damperMaxOpenTime;
      SabbathDelayHandler_t sabbathDelayHandler;
   } _private;
} ConvertibleCompartmentDamperPlugin_t;

void ConvertibleCompartmentDamperPlugin_Init(
   ConvertibleCompartmentDamperPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
