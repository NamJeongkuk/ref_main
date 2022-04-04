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
#include "SabbathData.h"
#include "GridData.h"
#include "SensorData.h"
#include "EvaporatorData.h"
#include "SystemMonitorData.h"

typedef struct
{
   const DefrostData_t *defrostData;
   const GridData_t *gridData;
   const SabbathData_t *sabbathData;
   const EvaporatorData_t *evaporatorData;
   const SensorData_t *sensorData;
   const SystemMonitorData_t *systemMonitorData;
} PersonalityParametricData_t;

const PersonalityParametricData_t *PersonalityParametricData_Get(I_DataModel_t *dataModel);

#endif
