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
#include "FanData.h"
#include "GridData.h"
#include "ConvertibleCompartmentData.h"
#include "SensorData.h"
#include "EvaporatorData.h"
#include "SystemMonitorData.h"
#include "EnhancedSabbathData.h"
#include "CompressorData.h"
#include "SetpointData.h"
#include "BspParametricConfiguration.h"
#include "SingleDamperData.h"
#include "DamperHeaterData.h"
#include "PulldownData.h"
#include "IceMakerData.h"
#include "FlowMeterData.h"

typedef struct
{
   const DefrostData_t *defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData;
   const CombinedFanData_t *fanData;
   const GridData_t *gridData;
   const SabbathData_t *sabbathData;
   const ConvertibleCompartmentData_t *convertibleCompartmentData;
   const EvaporatorData_t *evaporatorData;
   const SensorData_t *sensorData;
   const SystemMonitorData_t *systemMonitorData;
   const CompressorData_t *compressorData;
   const SetpointData_t *setpointData;
   const BspParametricConfiguration_t *bspConfigurationData;
   const SingleDamperData_t *freshFoodDamperData;
   const DamperHeaterData_t *damperHeaterData;
   const PulldownData_t *pulldownData;
   const IceMakerData_t *iceMakerData;
   const FlowMeterData_t *flowMeterData;
} PersonalityParametricData_t;

const PersonalityParametricData_t *PersonalityParametricData_Get(I_DataModel_t *dataModel);

#endif
