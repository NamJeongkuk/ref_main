/*!
 * @file  SensorFilteredReading.h
 * @brief 1. unfiltered value gets clamped using slewRate before EWMA filter
 *        2. multiplies slewRateDegFx100 by EWMA alpha to account for Step 1
 *        3. EWMA filter alpha value uses 1 - alpha from parametric due to ApplCommon equation difference
 *        4. All internal filter calculations are done using int32_t scaled up by 32768 from int16_t and DegFx100 to increase precision
 *        5. When writing to Erds values get scaled down to int16_t and scaled down by 32768 back to DegFx100 format
 *
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SENSORFILTEREDREADING_H
#define SENSORFILTEREDREADING_H

#include "I_Adc.h"
#include "I_Filter.h"
#include "Timer.h"
#include "I_DataModel.h"
#include "I_Mapper.h"
#include "EventSubscription.h"
#include "SensorData.h"
#include "ConvertibleCompartmentStateType.h"

enum
{
   SensorIsGood = true,
   SensorIsBad = false
};

typedef struct
{
   bool sensorState;
   uint16_t goodReadingCounter;
   uint16_t badReadingCounter;
   int16_t fallbackValue;
   uint16_t goodReadingMaxValue;
   uint16_t badReadingMaxValue;
} SensorFilteredReadingFallbackData_t;

typedef struct
{
   bool slewRateEnabled;
   int32_t slewRatePerSecondx100;
} SensorFilteredReadingClampData_t;

typedef struct
{
   Erd_t rawAdcCountErd;
   Erd_t unfilteredOutputErd;
   Erd_t filteredOutputErd;
} SensorFilteredReadingErds_t;

typedef struct
{
   SensorFilteredReadingErds_t erds;
   I_Filter_t *filter;
   bool filterHasBeenSeeded;
   I_Mapper_t *adcMapper;
   SensorFilteredReadingClampData_t clampData;
   SensorFilteredReadingFallbackData_t fallbackData;
   uint16_t filterInvalidValue;
   uint16_t filterAlphax100;
} SensorFilteredReadingChannelData_t;

typedef struct
{
   SensorFilteredReadingChannelData_t *channelData;
   uint8_t channelDataCount;
   uint8_t convertibleCompartmentIndex;
   Erd_t timerModule;
   Erd_t convertibleCompartmentStateErd;
   TimerTicks_t periodicCheckRateInMsec;
   const SensorDataConvertibleCompartmentSensorType_t *convertibleCompartmentSensorData;
} SensorFilteredReadingConfiguration_t;

typedef struct
{
   struct
   {
      const SensorFilteredReadingConfiguration_t *configuration;
      I_DataModel_t *dataModel;
      Timer_t filterTimer;
      EventSubscription_t onConvertibleCompartmentStateChanged;
   } _private;
} SensorFilteredReading_t;

void SensorFilteredReading_Init(
   SensorFilteredReading_t *instance,
   I_DataModel_t *dataMode,
   SensorFilteredReadingConfiguration_t *config);

#endif
