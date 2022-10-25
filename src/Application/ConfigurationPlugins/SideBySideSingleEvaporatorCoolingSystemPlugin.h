/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H
#define SIDEBYSIDESINGLEEVAPORATORCOOLINGSYSTEMPLUGIN_H

#include "I_DataModel.h"
#include "GridPlugin.h"
#include "ConstArrayMap_SideBySideSingleEvap.h"
#include "CompressorPlugin.h"
#include "DefrostPlugin.h"
#include "FreshFoodDamperPlugin.h"
#include "FreshFoodShiftOffsetCalculatorPlugin.h"
#include "FreezerShiftOffsetCalculatorPlugin.h"

typedef struct
{
   struct
   {
      GridPlugin_t gridPlugin;
      ConstArrayMap_SideBySideSingleEvap_t coolingStateBasedGridVotesTable;
      CompressorPlugin_t compressorPlugin;
      DefrostPlugin_t defrostPlugin;
      FreshFoodDamperPlugin_t freshFoodDamperPlugin;
      FreshFoodShiftOffsetCalculatorPlugin_t freshFoodShiftOffsetCalculatorPlugin;
      FreezerShiftOffsetCalculatorPlugin_t freezerShiftOffsetCalculatorPlugin;
   } _private;
} SideBySideSingleEvaporatorCoolingSystemPlugin_t;

void SideBySideSingleEvaporatorCoolingSystemPlugin_Init(SideBySideSingleEvaporatorCoolingSystemPlugin_t *instance, I_DataModel_t *dataModel);

#endif
