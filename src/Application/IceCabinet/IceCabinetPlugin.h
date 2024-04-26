/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETPLUGIN_H
#define ICECABINETPLUGIN_H

#include "I_DataModel.h"
#include "IceCabinetForcedCoolingFreezerEvaporatorFanVoter.h"
#include "IceCabinetFreezerEvapFanDependencyVoting.h"
#include "IceCabinetGridPlugin.h"
#include "IceCabinetGammaCalculator.h"
#include "FreezerSetpointToIceCabinetSetpointMapper.h"

typedef struct
{
   struct
   {
      IceCabinetForcedCoolingFreezerEvaporatorFanVoter_t iceCabinetForcedCoolingFreezerEvaporatorFanVoter;
      FreezerSetpointToIceCabinetSetpointMapper_t freezerSetpointToIceCabinetSetpointMapper;
      IceCabinetFreezerEvapFanDependencyVoting_t iceCabinetFreezerEvapFanDependencyVoting;
      IceCabinetGammaCalculator_t gammaCalculator;
      IceCabinetGridPlugin_t iceCabinetGridPlugin;
   } _private;
} IceCabinetPlugin_t;

void IceCabinetPlugin_Init(
   IceCabinetPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
