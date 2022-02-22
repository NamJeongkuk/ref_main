/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERSONALITYPARAMETRICDATA_H
#define PERSONALITYPARAMETRICDATA_H

#include "I_DataModel.h"
#include "DefrostData.h"

typedef struct
{
   const DefrostData_t *defrostData;
} PersonalityParametricData_t;

const PersonalityParametricData_t *PersonalityParametricData_Get(I_DataModel_t *dataModel);

#endif
