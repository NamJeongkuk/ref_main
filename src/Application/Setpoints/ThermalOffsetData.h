/*!
 * @file
 * @brief thermal offset parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef THERMALOFFSETDATA_H
#define THERMALOFFSETDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   uint8_t relativeHumidityTriggerForHighAmbientOffsetInPercent;
   int8_t userSetpointTriggerForHighAmbientOffsetInDegF;
   TemperatureDegFx100_t offsetForHighAmbientHumidityInDegFx100;
} HighAmbientOffsetData_t;

typedef struct
{
   int16_t slopeCoefficient;
   TemperatureDegFx100_t minimumAmbientOffsetInDegFx100;
   TemperatureDegFx100_t maximumAmbientOffsetInDegFx100;
} CrossAmbientOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t coldOffsetInDegFx100;
   TemperatureDegFx100_t midOffsetInDegFx100;
   TemperatureDegFx100_t warmOffsetInDegFx100;
} SetpointOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t mode1OffsetInDegFx100;
   TemperatureDegFx100_t mode2OffsetInDegFx100;
   TemperatureDegFx100_t mode3OffsetInDegFx100;
   TemperatureDegFx100_t mode4OffsetInDegFx100;
   TemperatureDegFx100_t mode5OffsetInDegFx100;
   TemperatureDegFx100_t mode6OffsetInDegFx100;
   TemperatureDegFx100_t mode7OffsetInDegFx100;
} FeaturePanSetpointOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t maxShiftInDegFx100;
   TemperatureDegFx100_t minShiftInDegFx100;
   TemperatureDegFx100_t upperAdjustmentLimitInDegFx100;
   TemperatureDegFx100_t lowerAdjustmentLimitInDegFx100;
   TemperatureDegFx100_t postDefrostShiftInDegFx100;
} ShiftOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   TemperatureDegFx100_t pulldownOffsetInDegFx100;
   const CrossAmbientOffsetData_t *crossAmbientOffsetData;
   const HighAmbientOffsetData_t *highAmbientOffsetData;
   const SetpointOffsetData_t *setpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} FreshFoodThermalOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   const CrossAmbientOffsetData_t *crossAmbientOffsetData;
   const HighAmbientOffsetData_t *highAmbientOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} FreezerThermalOffsetData_t;

typedef struct
{
   const FeaturePanSetpointOffsetData_t *setpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
   const CrossAmbientOffsetData_t *crossAmbientAsFreshFoodOffsetData;
   const CrossAmbientOffsetData_t *crossAmbientAsFreezerOffsetData;
} FeaturePanThermalOffsetData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   TemperatureDegFx100_t iceFormationOffsetInDegFx100;
   const CrossAmbientOffsetData_t *crossAmbientOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} IceCabinetThermalOffsetData_t;

typedef struct
{
   uint8_t longTermBeta;
   uint8_t longTermAverageUpdateTimeInMinutes;
   uint8_t updateTimeInMinutes;
} ShiftOffsetCalculatorData_t;

#endif
