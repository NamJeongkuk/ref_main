/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETPLUGIN_H
#define ICECABINETPLUGIN_H

#include "I_DataModel.h"
#include "IceCabinetFanPlugin.h"
#include "IceCabinetForcedCoolingFreezerEvaporatorFanVoter.h"
#include "IceCabinetGammaCalculator.h"
#include "IceCabinetGridPlugin.h"
#include "IceCabinetFanStartupSpeedVoting.h"
#include "IceCabinetFreezerEvapFanDependencyVoting.h"
#include "IceCabinetHeaterControl.h"
#include "FreezerSetpointToIceCabinetSetpointMapper.h"
#include "ErdResolver.h"

typedef struct
{
   struct
   {
      IceCabinetFanStartupSpeedVoting_t iceCabinetFanStartupSpeedVoting;
      IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t iceCabinetForcedCoolingFreezerEvaporatorFanVoter;
      FreezerSetpointToIceCabinetSetpointMapper_t freezerSetpointToIceCabinetSetpointMapper;
      IceCabinetFreezerEvapFanDependencyVoting_t iceCabinetFreezerEvapFanDependencyVoting;
      IceCabinetGammaCalculator_t gammaCalculator;
      IceCabinetGridPlugin_t iceCabinetGridPlugin;
      ErdResolver_t iceCabinetGasketHeaterErdResolver;
      IceCabinetHeaterControl_t iceCabinetHeaterControl;
   } _private;
} IceCabinetPlugin_t;

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
