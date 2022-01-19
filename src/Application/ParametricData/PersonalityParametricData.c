/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PersonalityParametricData.h"
#include "SystemErds.h"

const PersonalityParametricData_t *PersonalityParametricData_Get(I_DataModel_t *dataModel)
{
   const PersonalityParametricData_t *personalityParametricData;
   DataModel_Read(dataModel, Erd_PersonalityParametricData, &personalityParametricData);
   return personalityParametricData;
}
