/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTDATA_TESTDOUBLE_H
#define DEFROSTDATA_TESTDOUBLE_H

#include "DefrostData.h"

void DefrostData_TestDouble_SetPrechillFreezerSetpointInDegFx100(DefrostData_t *instance, TemperatureDegFx100_t prechillFreezerSetpointInDegFx100);

void DefrostData_TestDouble_SetPrechillConvertibleCompartmentSetpointInDegFx100(DefrostData_t *instance, TemperatureDegFx100_t prechillConvertibleCompartmentSetpointInDegFx100);

void DefrostData_TestDouble_SetDefrostMaxHoldoffTimeInMinutes(DefrostData_t *instance, uint16_t defrostMaxHoldoffTimeInMinutes);

void DefrostData_TestDouble_SetMaxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds(DefrostData_t *instance, uint16_t maxPrechillHoldoffTimeAfterDefrostTimerSatisfiedInSeconds);

void DefrostData_TestDouble_SetFreshFoodOnlyPostDwellExitTimeInMinutes(DefrostData_t *instance, uint8_t freshFoodOnlyPostDwellExitTimeInMinutes);

void DefrostData_TestDouble_SetThreeWayValveTimePriorToPrechillCountsAsPrechillTime(DefrostData_t *instance, bool threeWayValveTimePriorToPrechillCountsAsPrechillTime);

void DefrostData_TestDouble_Init(DefrostData_t *instance);

#endif
