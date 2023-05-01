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
#include "DefrostStateOnCompareMatch.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "DefrostParameterSelector.h"
#include "DefrostStateOnCompareMatch.h"
#include "DefrostHeaterOnTimeCounter.h"
#include "FreezerDefrostHeaterVotingFrameworkPlugin.h"
#include "NextDefrostTypeArbiter.h"
#include "DefrostTestRequestHandler.h"
#include "ReadyToDefrost.h"

typedef struct
{
   struct
   {
      Defrost_t defrost;
      DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
      ReadyToDefrost_t readyToDefrost;
      TimeThatPrechillConditionsAreMet_t timeThatPrechillConditionsAreMet;
      DefrostParameterSelector_t defrostParameterSelector;
      DefrostStateOnCompareMatch_t defrostStateOnCompareMatch;
      DefrostHeaterOnTimeCounter_t defrostHeaterOnTimeCounter;
      FreezerDefrostHeaterVotingFrameworkPlugin_t freezerDefrostHeaterVotingFramework;
      NextDefrostTypeArbiter_t nextDefrostTypeArbiter;
      DefrostTestRequestHandler_t defrostTestRequestHandler;
   } _private;
} DefrostPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void DefrostPlugin_Init(DefrostPlugin_t *instance, I_DataModel_t *dataModel);

#endif
