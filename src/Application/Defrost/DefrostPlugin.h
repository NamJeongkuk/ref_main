/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTPLUGIN_H
#define DEFROSTPLUGIN_H

#include "I_DataModel.h"
#include "Defrost.h"
#include "DefrostHeaterMaxOnTime.h"
#include "FreshFoodOnlyDefrostArbitrator.h"
#include "DefrostCompressorOnTimeCounter.h"
#include "DefrostStateOnCompareMatch.h"
#include "DoorAccelerationCounter.h"
#include "ReadyToDefrost.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "DefrostParameterSelector.h"
#include "DefrostStateOnCompareMatch.h"
#include "ActivelyWaitingForDefrostOnCompareMatch.h"
#include "DefrostHeaterOnTimeCounter.h"
#include "FreezerDefrostHeaterVotingFrameworkPlugin.h"

typedef struct
{
   struct
   {
      Defrost_t defrost;
      DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
      FreshFoodOnlyDefrostArbitrator_t freshFoodOnlyDefrostArbitrator;
      DefrostCompressorOnTimeCounter_t defrostCompressorOnTimeCounter;
      DoorAccelerationCounter_t doorAccelerationCounter;
      ReadyToDefrost_t readyToDefrost;
      TimeThatPrechillConditionsAreMet_t timeThatPrechillConditionsAreMet;
      DefrostParameterSelector_t defrostParameterSelector;
      DefrostStateOnCompareMatch_t defrostStateOnCompareMatch;
      ActivelyWaitingForDefrostOnCompareMatch_t activelyWaitingForDefrostOnCompareMatch;
      DefrostHeaterOnTimeCounter_t defrostHeaterOnTimeCounter;
      FreezerDefrostHeaterVotingFrameworkPlugin_t freezerDefrostHeaterVotingFramework;
   } _private;
} DefrostPlugin_t;

void DefrostPlugin_Init(DefrostPlugin_t *instance, I_DataModel_t *dataModel);

#endif
