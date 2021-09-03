/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Adc.h"
#include "I_Adc.h"
#include "iodefine.h"
#include "uassert.h"
#include "Event_Null.h"
#include "ConstArrayMap_LinearSearch.h"
#include "utils.h"

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End - 1))

#define EXPAND_AS_CHANNEL_CONFIGURATION(name, port, bit, channel) \
   S12AD0.ADANS.BIT.CH = MAX(S12AD0.ADANS.BIT.CH, channel);

#define EXPAND_AS_ERD_ADC_CHANNEL_PAIRS(name, port, bit, channel) \
   {                                                              \
      name, channel                                               \
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

static const ErdAdcChannelPair_t erdAdcChannelPairs[] = {
   ADC_TABLE(EXPAND_AS_ERD_ADC_CHANNEL_PAIRS)
};

static const ConstArrayMap_LinearSearchConfiguration_t erdToAdcChannelMapConfiguration = {
   erdAdcChannelPairs,
   NUM_ELEMENTS(erdAdcChannelPairs),
   ELEMENT_SIZE(erdAdcChannelPairs),
   MEMBER_SIZE(ErdAdcChannelPair_t, erd),
   OFFSET_OF(ErdAdcChannelPair_t, erd)
};

static struct
{
   I_DataSource_t interface;
   ConstArrayMap_LinearSearch_t erdToChannelMap;
} instance;

static void ConfigureS12ad0(void)
{
   // Enable the ADC module
   MSTP(S12AD) = 0;
   MSTP(S12AD0) = 0;

   // Continuous scan
   S12AD0.ADCSR.BIT.ADCS = 2;

   // PCLK / 8
   S12AD0.ADCSR.BIT.CKS = 0;
   ADC_TABLE(EXPAND_AS_CHANNEL_CONFIGURATION)

   // A/D data alignment left
   S12AD0.ADCER.BIT.ADRFMT = 1;

   // Start ADC
   S12AD0.ADCSR.BIT.ADST = 1;
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   volatile AdcCounts_t *result = (volatile uint16_t *)&S12AD0.ADDR0A;

   if(channel < HardwareChannelCount)
   {
      return *(result + channel);
   }

   return 0;
}

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   uint16_t index;
   ErdAdcChannelPair_t pair;

   if(ConstArrayMap_Find(&instance.erdToChannelMap.interface, &erd, &index, &pair))
   {
      AdcCounts_t value = ReadAdcChannel(pair.channel);
      memcpy(data, &value, sizeof(AdcCounts_t));
   }
   else
   {
      uassert(!"ERD not found");
   }
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(const I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   uassert(ERD_IS_IN_RANGE(erd));

   return sizeof(AdcCounts_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_Adc_Init(void)
{
   ConstArrayMap_LinearSearch_Init(
      &instance.erdToChannelMap,
      &erdToAdcChannelMapConfiguration);

   instance.interface.api = &api;
   instance.interface.OnDataChange = Event_Null_GetInstance();

   ConfigureS12ad0();

   return &instance.interface;
}
