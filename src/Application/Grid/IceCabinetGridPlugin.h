/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICECABINETGRIDPLUGIN_H
#define ICECABINETGRIDPLUGIN_H

#include "Grid.h"
#include "I_DataModel.h"
#include "GridLineCalculator.h"
#include "GridBlockCalculator.h"
#include "OverrideArbiter.h"
#include "CrossAmbientHysteresisAdjustmentCalculator.h"

typedef struct
{
   Grid_t grid;
   GridLineCalculator_t gridLineCalculator;
   GridBlockCalculator_t gridBlockCalculator;
   OverrideArbiter_t gridBlockNumberArbiter;
} IceCabinetGridPlugin_t;

void IceCabinetGridPlugin_Init(IceCabinetGridPlugin_t *instance, I_DataModel_t *dataModel);

#endif
