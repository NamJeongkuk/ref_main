/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PERSONALITYPARAMETRICDATA_TESTDOUBLE_H
#define PERSONALITYPARAMETRICDATA_TESTDOUBLE_H

#include "PersonalityParametricData.h"

void PersonalityParametricData_TestDouble_SetEvaporator(PersonalityParametricData_t *instance, const EvaporatorData_t *evaporatorData);

void PersonalityParametricData_TestDouble_SetDefrost(PersonalityParametricData_t *instance, const DefrostData_t *defrostData);

void PersonalityParametricData_TestDouble_SetFans(PersonalityParametricData_t *instance, const FanData_t *fanData);

void PersonalityParametricData_TestDouble_SetGrid(PersonalityParametricData_t *instance, const GridData_t *gridData);

void PersonalityParametricData_TestDouble_SetSabbath(PersonalityParametricData_t *instance, const SabbathData_t *sabbathData);

void PersonalityParametricData_TestDouble_SetConvertibleCompartment(PersonalityParametricData_t *instance, const ConvertibleCompartmentData_t *convertibleCompartmentData);

void PersonalityParametricData_TestDouble_SetSensors(PersonalityParametricData_t *instance, const SensorData_t *sensorData);

void PersonalityParametricData_TestDouble_SetSystemMonitor(PersonalityParametricData_t *instance, const SystemMonitorData_t *systemMonitorData);

void PersonalityParametricData_TestDouble_SetEnhancedSabbath(PersonalityParametricData_t *instance, const EnhancedSabbathData_t *enhancedSabbathData);

void PersonalityParametricData_TestDouble_Init(PersonalityParametricData_t *instance);

#endif
