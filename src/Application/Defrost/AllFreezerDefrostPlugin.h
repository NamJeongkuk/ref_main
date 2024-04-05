/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALLFREEZERDEFROSTPLUGIN_H
#define ALLFREEZERDEFROSTPLUGIN_H

#include "I_DataModel.h"
#include "Defrost.h"
#include "DefrostHeaterMaxOnTime.h"
#include "DefrostStateOnCompareMatch.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "DefrostStateOnCompareMatch.h"
#include "DefrostHeaterOnTimeCounter.h"
#include "FreezerDefrostHeaterVotingFrameworkPlugin.h"
#include "DefrostTestRequestHandler.h"
#include "ReadyToDefrost.h"
#include "SabbathReadyToDefrost.h"

typedef struct
{
   struct
   {
      Defrost_t defrost;
      DefrostHeaterMaxOnTime_t defrostHeaterMaxOnTime;
      ReadyToDefrost_t readyToDefrost;
      TimeThatPrechillConditionsAreMet_t timeThatPrechillConditionsAreMet;
      DefrostStateOnCompareMatch_t defrostStateOnCompareMatch;
      DefrostHeaterOnTimeCounter_t defrostHeaterOnTimeCounter;
      FreezerDefrostHeaterVotingFrameworkPlugin_t freezerDefrostHeaterVotingFramework;
      DefrostTestRequestHandler_t defrostTestRequestHandler;
      SabbathReadyToDefrost_t sabbathReadyToDefrost;
   } _private;
} AllFreezerDefrostPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void AllFreezerDefrostPlugin_Init(AllFreezerDefrostPlugin_t *instance, I_DataModel_t *dataModel);

#endif
