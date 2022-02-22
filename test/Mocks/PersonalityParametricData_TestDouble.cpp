/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PersonalityParametricData_TestDouble.h"
#include <string.h>

void PersonalityParametricData_TestDouble_SetDefrost(PersonalityParametricData_t *instance, const DefrostData_t *defrostData)
{
   instance->defrostData = defrostData;
}

void PersonalityParametricData_TestDouble_Init(PersonalityParametricData_t *instance)
{
   memset(instance, 0, sizeof(PersonalityParametricData_t));
}
