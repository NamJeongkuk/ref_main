/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorFilteredReadingPlugin.h"
#include "PersonalityParametricData.h"
#include "utils.h"

void SensorFilteredReadingPlugin_Init(I_DataModel_t *dataModel)
{
   const SensorData_t *sensorData;
   sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
   IGNORE(sensorData);
}
