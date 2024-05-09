/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FOURDOORDUALEVAPDEFROSTPLUGIN_H
#define FOURDOORDUALEVAPDEFROSTPLUGIN_H

#include "I_DataModel.h"
#include "Defrost.h"
#include "DefrostHeaterMaxOnTime.h"
#include "DefrostStateOnCompareMatch.h"
#include "TimeThatPrechillConditionsAreMet.h"
#include "DefrostStateOnCompareMatch.h"
#include "DefrostHeaterOnTimeCounter.h"
#include "FreshFoodDefrostHeaterVotingFrameworkPlugin.h"
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
      DefrostHeaterOnTimeCounter_t freshFoodDefrostHeaterOnTimeCounter;
      DefrostHeaterOnTimeCounter_t freezerDefrostHeaterOnTimeCounter;
      FreshFoodDefrostHeaterVotingFrameworkPlugin_t freshFoodDefrostHeaterVotingFramework;
      FreezerDefrostHeaterVotingFrameworkPlugin_t freezerDefrostHeaterVotingFramework;
      DefrostTestRequestHandler_t defrostTestRequestHandler;
      SabbathReadyToDefrost_t sabbathReadyToDefrost;
   } _private;
} FourDoorDualEvapDefrostPlugin_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void FourDoorDualEvapDefrostPlugin_Init(FourDoorDualEvapDefrostPlugin_t *instance, I_DataModel_t *dataModel);

#endif
