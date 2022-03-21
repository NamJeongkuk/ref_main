/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERSONALITYPARAMETRICDATA_TESTDOUBLE_H
#define PERSONALITYPARAMETRICDATA_TESTDOUBLE_H

#include "PersonalityParametricData.h"

void PersonalityParametricData_TestDouble_SetDefrost(PersonalityParametricData_t *instance, const DefrostData_t *defrostData);

void PersonalityParametricData_TestDouble_SetGrid(PersonalityParametricData_t *instance, const GridData_t *gridData);

void PersonalityParametricData_TestDouble_SetSabbath(PersonalityParametricData_t *instance, const SabbathData_t *sabbathData);

void PersonalityParametricData_TestDouble_Init(PersonalityParametricData_t *instance);

#endif
