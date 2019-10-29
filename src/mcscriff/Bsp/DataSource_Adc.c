/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Adc.h"
#include "I_Adc.h"
#include "uassert.h"
#include "Event_Null.h"
#include "utils.h"
#include <string.h>

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End))

#define EXPAND_AS_ERD_ADC_CHANNEL_PAIRS(name, port, bit, channel) \
   {                                                              \
      Erd_BspAdc_##name, channel                                  \
   },

enum
{
   HardwareChannelCount = 32
};

typedef struct
{
   Erd_t erd;
   AdcChannel_t channel;
} ErdAdcChannelPair_t;

static const ErdAdcChannelPair_t erdAdcChannelPairs[] =
   {
      ADC_TABLE(EXPAND_AS_ERD_ADC_CHANNEL_PAIRS)
   };

static const ConstArrayMap_LinearSearchConfiguration_t erdToAdcChannelMapConfiguration =
   {
      erdAdcChannelPairs,
      NUM_ELEMENTS(erdAdcChannelPairs),
      ELEMENT_SIZE(erdAdcChannelPairs),
      MEMBER_SIZE(ErdAdcChannelPair_t, erd),
      OFFSET_OF(ErdAdcChannelPair_t, erd)
   };

static void ConfigurePins(void)
{
}

static void ConfigureAdc(void)
{
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   IGNORE(channel);
   return 0;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   uint16_t index;
   ErdAdcChannelPair_t pair;

   REINTERPRET(instance, _instance, DataSource_Adc_t *);
   uassert(ERD_IS_IN_RANGE(erd));

   if(ConstArrayMap_Find(&instance->_private.erdToChannelMap.interface, &erd, &index, &pair))
   {
      AdcCounts_t value = ReadAdcChannel(pair.channel);
      memcpy(data, &value, sizeof(AdcCounts_t));
   }
   else
   {
      uassert(!"ERD not found");
   }
}

static void Write(I_DataSource_t *instance, const Erd_t erd, const void *data)
{
   IGNORE(instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *instance, const Erd_t erd)
{
   IGNORE(instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(AdcCounts_t);
}

static const I_DataSource_Api_t api =
   { Read, Write, Has, SizeOf };

void DataSource_Adc_Init(DataSource_Adc_t *instance)
{
   ConstArrayMap_LinearSearch_Init(
      &instance->_private.erdToChannelMap,
      &erdToAdcChannelMapConfiguration);

   instance->interface.api = &api;
   instance->interface.OnDataChange = Event_Null_GetInstance();

   ConfigurePins();
   ConfigureAdc();
}
