/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "PersonalityParametricData_TestDouble.h"
#include <string.h>

void PersonalityParametricData_TestDouble_SetEvaporator(PersonalityParametricData_t *instance, const EvaporatorData_t *evaporatorData)
{
   instance->evaporatorData = evaporatorData;
}

void PersonalityParametricData_TestDouble_SetDefrost(PersonalityParametricData_t *instance, const DefrostData_t *defrostData)
{
   instance->defrostData = defrostData;
}

void PersonalityParametricData_TestDouble_SetFans(PersonalityParametricData_t *instance, const CombinedFanData_t *fanData)
{
   instance->fanData = fanData;
}

void PersonalityParametricData_TestDouble_SetGrid(PersonalityParametricData_t *instance, const GridData_t *gridData)
{
   instance->gridData = gridData;
}

void PersonalityParametricData_TestDouble_SetSabbath(PersonalityParametricData_t *instance, const SabbathData_t *sabbathData)
{
   instance->sabbathData = sabbathData;
}

void PersonalityParametricData_TestDouble_SetConvertibleCompartment(PersonalityParametricData_t *instance, const ConvertibleCompartmentData_t *convertibleCompartmentData)
{
   instance->convertibleCompartmentData = convertibleCompartmentData;
}

void PersonalityParametricData_TestDouble_SetSensors(PersonalityParametricData_t *instance, const SensorData_t *sensorData)
{
   instance->sensorData = sensorData;
}

void PersonalityParametricData_TestDouble_SetSystemMonitor(PersonalityParametricData_t *instance, const SystemMonitorData_t *systemMonitorData)
{
   instance->systemMonitorData = systemMonitorData;
}

void PersonalityParametricData_TestDouble_SetEnhancedSabbath(PersonalityParametricData_t *instance, const EnhancedSabbathData_t *enhancedSabbathData)
{
   instance->enhancedSabbathData = enhancedSabbathData;
}

void PersonalityParametricData_TestDouble_Init(PersonalityParametricData_t *instance)
{
   memset(instance, 0, sizeof(PersonalityParametricData_t));
}
