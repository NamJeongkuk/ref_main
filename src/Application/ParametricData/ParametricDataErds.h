/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARAMETRICDATAERDS_H
#define PARAMETRICDATAERDS_H

#include "I_DataModel.h"
#include "I_Action.h"

void ParametricDataErds_Init(
   I_DataModel_t *dataModel,
   Erd_t personalityIdErd,
   Erd_t personalityParametricErd,
   Erd_t personalityIdOutOfRangeErd,
   I_Action_t *jumpToBootloaderAction);

#endif
