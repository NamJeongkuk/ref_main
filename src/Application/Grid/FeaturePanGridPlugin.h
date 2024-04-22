/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FEATUREPANGRIDPLUGIN_H
#define FEATUREPANGRIDPLUGIN_H

#include "Grid.h"
#include "I_DataModel.h"
#include "GridLineCalculator.h"
#include "GridBlockCalculator.h"
#include "GridOffsetAdder.h"
#include "OverrideArbiter.h"
#include "CrossAmbientHysteresisAdjustmentCalculator.h"
#include "ConstArrayMap_FeaturePan.h"

typedef struct
{
   Grid_t grid;
   GridLineCalculator_t gridLineCalculator;
   GridBlockCalculator_t gridBlockCalculator;
   OverrideArbiter_t gridBlockNumberArbiter;
   CrossAmbientHysteresisAdjustmentCalculator_t crossAmbientHysteresisAdjustmentCalculator;
   ConstArrayMap_FeaturePan_t featurePanCoolingStateBasedGridVotesTable;
} FeaturePanGridPlugin_t;

void FeaturePanGridPlugin_Init(FeaturePanGridPlugin_t *instance, I_DataModel_t *dataModel);

#endif
