/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "FanPlugin.h"
#include "PersonalityParametricData.h"
#include "FanData.h"
#include "SystemErds.h"
#include "utils.h"

void FanPlugin_Init(I_DataModel_t *dataModel)
{
   const FanData_t *fanData = PersonalityParametricData_Get(dataModel)->fanData;
   IGNORE(fanData);
}
