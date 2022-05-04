/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SensorFilteredReadingPlugin.h"
#include "PersonalityParametricData.h"
#include "Mapper_LookupTable.h"
#include "SensorFilteredReading.h"
#include "Filter_ExponentiallyWeightedMovingAverage.h"
#include "SystemErds.h"
#include "utils.h"
#include "uassert.h"

// (type, sensorType , countErd, unfilteredErd, filteredErd, IsSigned)
#define SENSOR_FILTERING_TABLE_ALL(ENTRY)                                                                                                                  \
   ENTRY(Local, fzCabinetThermistor, Erd_FreezerThermistor_AdcCount, Erd_Freezer_UnfilteredTemperature, Erd_Freezer_FilteredTemperature)                   \
   ENTRY(Local, ffCabinetThermistor, Erd_FreshFoodThermistor_AdcCount, Erd_FreshFood_UnfilteredTemperature, Erd_FreshFood_FilteredTemperature)             \
   ENTRY(Local, fzEvapThermistor, Erd_FreezerEvapThermistor_AdcCount, Erd_FreezerEvap_UnfilteredTemperature, Erd_FreezerEvap_FilteredTemperature)          \
   ENTRY(Local, ffEvapThermistor, Erd_FreshFoodEvapThermistor_AdcCount, Erd_FreshFoodEvap_UnfilteredTemperature, Erd_FreshFoodEvap_FilteredTemperature)    \
   ENTRY(Local, ccCabinetThermistor, Erd_CcCompartmentThermistor_AdcCount, Erd_CcCompartment_UnfilteredTemperature, Erd_CcCompartment_FilteredTemperature) \
   ENTRY(Local, ambientThermistor, Erd_AmbientThermistor_AdcCount, Erd_Ambient_UnfilteredTemperature, Erd_Ambient_FilteredTemperature)                     \
   ENTRY(Local, ccEvapThermistor, Erd_CcEvapThermistor_AdcCount, Erd_CcEvap_UnfilteredTemperature, Erd_CcEvap_FilteredTemperature)

// (type, sensorType)
#define SENSOR_FILTERING_TABLE_NON_CC_CABINET(ENTRY) \
   ENTRY(Local, fzCabinetThermistor)                 \
   ENTRY(Local, ffCabinetThermistor)                 \
   ENTRY(Local, fzEvapThermistor)                    \
   ENTRY(Local, ffEvapThermistor)                    \
   ENTRY(Local, ambientThermistor)                   \
   ENTRY(Local, ccEvapThermistor)

// clang-format off
#define EXPAND_AS_SET_CHANNEL_DATA(type, sensorType, countErd, unfilteredErd, filteredErd)                                                                             \
   instance.sensorChannelData[ChannelConfig_##sensorType].adcMapper = &instance.channelReadingMapper[ChannelConfig_##sensorType].interface;                            \
   instance.sensorChannelData[ChannelConfig_##sensorType].filterInvalidValue =                                                                                         \
      instance.sensorData->sensorType->lookupTable->mappings[instance.sensorData->sensorType->lookupTable->mappingCount - 1].y;                                        \
   instance.sensorChannelData[ChannelConfig_##sensorType].filter = &instance.filter[ChannelConfig_##sensorType].interface;                                             \
   instance.sensorChannelData[ChannelConfig_##sensorType].filterAlphax100 =                                                                                            \
      ((instance.sensorData->sensorType->alphaNum * Hundred) / instance.sensorData->sensorType->alphaDenom);                                                           \
                                                                                                                                                                       \
   instance.sensorChannelData[ChannelConfig_##sensorType].clampData.slewRateEnabled = instance.sensorData->sensorType->clampData.slewRateEnabled;                      \
   instance.sensorChannelData[ChannelConfig_##sensorType].clampData.slewRatePerSecondx100 = (int32_t)instance.sensorData->sensorType->clampData.slewRatePerSecondx100; \
                                                                                                                                                                       \
   instance.sensorChannelData[ChannelConfig_##sensorType].erds.rawAdcCountErd = countErd;                                                                              \
   instance.sensorChannelData[ChannelConfig_##sensorType].erds.unfilteredOutputErd = unfilteredErd;                                                                    \
   instance.sensorChannelData[ChannelConfig_##sensorType].erds.filteredOutputErd = filteredErd;                                                                        \
   \
   instance.sensorChannelData[ChannelConfig_##sensorType].fallbackData.goodReadingMaxValue = instance.sensorData->sensorType->goodReadingCounterMax;                   \
   instance.sensorChannelData[ChannelConfig_##sensorType].fallbackData.badReadingMaxValue = instance.sensorData->sensorType->badReadingCounterMax;                     \
                                                                                                                                                                       \
   Filter_ExponentiallyWeightedMovingAverage_Init(                                                                                                                     \
      &instance.filter[ChannelConfig_##sensorType],                                                                                                                    \
      instance.sensorData->sensorType->windowSize,                                                                                                                     \
      IS_SIGNED(int32_t),                                                                                                                                              \
      sizeof(int32_t),                                                                                                                                                 \
      instance.sensorData->sensorType->alphaDenom - instance.sensorData->sensorType->alphaNum,                                                                         \
      instance.sensorData->sensorType->alphaDenom);                                                                                                                    \
   Mapper_LookupTable_Init(&instance.channelReadingMapper[ChannelConfig_##sensorType], instance.sensorData->sensorType->lookupTable);
// clang-format on

#define EXPAND_AS_CHANNEL_SET_DATA_FALLBACK_VALUES_NON_CC_CABINET(type, sensorType) \
   instance.sensorChannelData[ChannelConfig_##sensorType].fallbackData.fallbackValue = instance.sensorData->sensorType->fallbackValueDegFx100;

#define EXPAND_AS_CHANNEL_ENUM(type, sensorType, countErd, unfilteredErd, filteredErd) \
   ChannelConfig_##sensorType,

enum
{
   SENSOR_FILTERING_TABLE_ALL(EXPAND_AS_CHANNEL_ENUM)
      ChannelConfig_MaxNumberOfChannels
};

enum
{
   Hundred = 100
};

typedef struct
{
   SensorFilteredReading_t sensorFilteredReadingInstance;
   SensorFilteredReadingChannelData_t sensorChannelData[ChannelConfig_MaxNumberOfChannels];
   SensorFilteredReadingConfiguration_t sensorConfig;
   Filter_ExponentiallyWeightedMovingAverage_t filter[ChannelConfig_MaxNumberOfChannels];
   Mapper_LookupTable_t channelReadingMapper[ChannelConfig_MaxNumberOfChannels];
   const SensorData_t *sensorData;
} SensorFilteredReadingPlugin_t;

static SensorFilteredReadingPlugin_t instance;

void SensorFilteredReadingPlugin_Init(I_DataModel_t *dataModel)
{
   instance.sensorData = PersonalityParametricData_Get(dataModel)->sensorData;
   SENSOR_FILTERING_TABLE_ALL(EXPAND_AS_SET_CHANNEL_DATA);
   SENSOR_FILTERING_TABLE_NON_CC_CABINET(EXPAND_AS_CHANNEL_SET_DATA_FALLBACK_VALUES_NON_CC_CABINET);

   instance.sensorConfig.channelData = instance.sensorChannelData;
   instance.sensorConfig.channelDataCount = ChannelConfig_MaxNumberOfChannels;
   instance.sensorConfig.timerModule = Erd_TimerModule;
   instance.sensorConfig.convertibleCompartmentCabinetIndex = ChannelConfig_ccCabinetThermistor;
   instance.sensorConfig.ccCabinetStateErd = Erd_CcCabinetState;
   instance.sensorConfig.ccSensorData = PersonalityParametricData_Get(dataModel)->sensorData->ccCabinetThermistor;
   instance.sensorConfig.periodicCheckRateInMsec = instance.sensorData->periodicUpdateRate;

   SensorFilteredReading_Init(
      &instance.sensorFilteredReadingInstance,
      dataModel,
      &instance.sensorConfig);
}
