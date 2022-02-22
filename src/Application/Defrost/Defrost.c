/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Defrost.h"
#include "PersonalityParametricData.h"

void Defrost_Init(Defrost_t *instance, I_DataModel_t *dataModel)
{
   instance->_private.defrostParametricData = PersonalityParametricData_Get(dataModel)->defrostData;
}
