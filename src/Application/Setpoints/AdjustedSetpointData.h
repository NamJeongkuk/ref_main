/*!
 * @file
 * @brief adjusted setpoint parametric data
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADJUSTEDSETPOINTDATA_H
#define ADJUSTEDSETPOINTDATA_H

#include "TemperatureDegFx100.h"

typedef struct
{
   int16_t slopeCoefficient;
   TemperatureDegFx100_t minimumAmbientOffset;
   TemperatureDegFx100_t maximumAmbientOffset;
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
} FixedSetpointOffsetData_t;

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
   const SetpointOffsetData_t *setpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} FreshFoodAdjustedSetpointData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   const CrossAmbientOffsetData_t *crossAmbientOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} FreezerAdjustedSetpointData_t;

typedef struct
{
   const CrossAmbientOffsetData_t *crossAmbientAsFreshFoodOffsetData;
   const CrossAmbientOffsetData_t *crossAmbientAsFreezerOffsetData;
   const FixedSetpointOffsetData_t *fixedSetpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} ConvertibleCompartmentAdjustedSetpointData_t;

typedef struct
{
   const FixedSetpointOffsetData_t *fixedSetpointOffsetData;
   const ShiftOffsetData_t *shiftOffsetData;
} DeliPanAdjustedSetpointData_t;

typedef struct
{
   TemperatureDegFx100_t cabinetOffsetInDegFx100;
   TemperatureDegFx100_t iceFormationOffsetInDegFx100;
   const ShiftOffsetData_t *shiftOffsetData;
} IceBoxAdjustedSetpointData_t;

typedef struct
{
   uint8_t ambientTemperatureAverageWindowSize;
   uint8_t ambientTemperatureAverageUpdateIntervalInMinutes;
} CrossAmbientOffsetCalculatorData_t;

typedef struct
{
   uint8_t longTermBeta;
   uint8_t longTermAverageUpdateTimeInMinutes;
   uint8_t updateTimeInMinutes;
} ShiftOffsetCalculatorData_t;

typedef struct
{
   const FreshFoodAdjustedSetpointData_t *freshFoodAdjustedSetpointData;
   const FreezerAdjustedSetpointData_t *freezerAdjustedSetpointData;
   const ConvertibleCompartmentAdjustedSetpointData_t *convertibleCompartmentAdjustedSetpointData;
   const DeliPanAdjustedSetpointData_t *deliPanAdjustedSetpointData;
   const IceBoxAdjustedSetpointData_t *iceBoxAdjustedSetpointData;
   const CrossAmbientOffsetCalculatorData_t *crossAmbientOffsetCalculatorData;
   const ShiftOffsetCalculatorData_t *shiftOffsetCalculatorData;
} AdjustedSetpointData_t;

#endif
