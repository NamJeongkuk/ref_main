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
#include "IceCabinetData.h"
#include "IceCabinetHeaterData.h"
#include "FeaturePanData.h"
#include "SensorData.h"
#include "PlatformData.h"
#include "SystemMonitorData.h"
#include "EnhancedSabbathData.h"
#include "CompressorData.h"
#include "SetpointData.h"
#include "ThermalOffsetData.h"
#include "BspParametricConfiguration.h"
#include "DamperData.h"
#include "DamperHeaterData.h"
#include "PulldownData.h"
#include "IceMakerData.h"
#include "FlowMeterData.h"
#include "DispenserData.h"
#include "RecessHeaterData.h"
#include "RefrigeratorModel.h"
#include "LightingData.h"
#include "LoadOffDoorOpenData.h"
#include "FilterData.h"
#include "WaterValveFlowRate.h"
#include "CabinetTemperatureExceededData.h"
#include "SealedSystemValveData.h"
#include "FreshFoodNonHeatedCycleDefrostData.h"
#include "FeaturePanData.h"

enum
{
   PersonalityParametricData_UseParametricValue = 0
};

typedef struct
{
   const RefrigeratorModel_t *refrigeratorModelData;
   const PlatformData_t *platformData;
   const DefrostData_t *defrostData;
   const EnhancedSabbathData_t *enhancedSabbathData;
   const CombinedFanData_t *fanData;
   const GridData_t *freshFoodAndFreezerGridData;
   const DeltaGridLines_t *freshFoodAndFreezerDeltaGridLines;
   const TemperatureDegFx100_t freshFoodMinimumCrossAmbientAdjustedHysteresisInDegFx100;
   const uint16_t freshFoodCrossAmbientHysteresisCoefficientDegFx1000OverDegF;
   const GridData_t *featurePanGridData;
   const DeltaGridLines_t *featurePanDeltaGridLines;
   const GridData_t *iceCabinetGridData;
   const DeltaGridLines_t *iceCabinetDeltaGridLines;
   const SabbathData_t *sabbathData;
   const ConvertibleCompartmentData_t *convertibleCompartmentData;
   const FeaturePanData_t *featurePanData;
   const IceCabinetData_t *iceCabinetData;
   const IceCabinetHeaterData_t *ductDoorHeaterData;
   const IceCabinetHeaterData_t *iceCabinetGasketHeaterData;
   const IceCabinetHeaterData_t *iceDuctPortHeaterData;
   const SensorData_t *sensorData;
   const SystemMonitorData_t *systemMonitorData;
   const CompressorData_t *compressorData;
   const SetpointData_t *setpointData;
   const FreshFoodThermalOffsetData_t *freshFoodThermalOffsetData;
   const FreezerThermalOffsetData_t *freezerThermalOffsetData;
   const FeaturePanThermalOffsetData_t *featurePanThermalOffsetData;
   const IceCabinetThermalOffsetData_t *iceCabinetThermalOffsetData;
   const ShiftOffsetCalculatorData_t *shiftOffsetCalculatorData;
   const BspParametricConfiguration_t *bspConfigurationData;
   const DamperData_t *damperData;
   const DamperHeaterData_t *damperHeaterData;
   const PulldownData_t *pulldownData;
   const IceMakerData_t *iceMakerData;
   const FlowMeterData_t *flowMeterData;
   const DispenserData_t *dispenserData;
   const RecessHeaterData_t *recessHeaterData;
   const TurboModeSetpointData_t *turboModeSetpointData;
   const LightingData_t *lightingData;
   const LoadOffDoorOpenData_t *loadOffDoorOpenData;
   const FilterData_t *filterData;
   const WaterValveFlowRate_t *waterValveFlowRate;
   const CabinetTemperatureExceededData_t *cabinetTemperatureExceeded;
   const SealedSystemValveData_t *sealedSystemValveData;
   const FreshFoodNonHeatedCycleDefrostData_t *freshFoodNonHeatedCycleDefrost;
} PersonalityParametricData_t;

const PersonalityParametricData_t *PersonalityParametricData_Get(I_DataModel_t *dataModel);

#endif
