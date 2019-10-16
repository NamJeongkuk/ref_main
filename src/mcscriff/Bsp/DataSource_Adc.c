/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_Adc.h"
#include "I_Adc.h"
#include "iodefine.h"
#include "uassert.h"
#include "Event_Null.h"
#include "utils.h"
#include <string.h>

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End))

#define EXPAND_AS_SET_PIN_DIRECTIONS(name, port, bit, channel) \
   PORT##port.PDR.BIT.B##bit = 0;

#define EXPAND_AS_SET_PIN_MODES(name, port, bit, channel) \
   PORT##port.PMR.BIT.B##bit = 0;

#define EXPAND_AS_SET_PIN_FUNCTIONS(name, port, bit, channel) \
   MPC.P##port##bit##PFS.BIT.ASEL = 1;

#define EXPAND_AS_ENABLE_CHANNELS_FOR_CONVERSION(name, port, bit, channel) \
   ((channel < 8) ? (BIT_SET(S12AD.ADANSA0.WORD, channel)) : (BIT_SET(S12AD.ADANSA0.WORD, channel)));

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
   /*
   // Enable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 0;

   // Enable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 1;

   ADC_TABLE(EXPAND_AS_SET_PIN_DIRECTIONS)
   ADC_TABLE(EXPAND_AS_SET_PIN_MODES)
   ADC_TABLE(EXPAND_AS_SET_PIN_FUNCTIONS)

   // Disable writing to the PFS register
   MPC.PWPR.BIT.PFSWE = 0;

   // Disable writing to the PFSWE bit
   MPC.PWPR.BIT.B0WI = 1;
   */
}

static void ConfigureAdcGroupA(void)
{
   /*
   // PRCR write enabled
   SYSTEM.PRCR.WORD = 0xA50F;

   // Enable ADC module
   MSTP(S12AD) = 0;

   // Write Disabled
   SYSTEM.PRCR.WORD = 0xA500;

   // Disable trigger
   S12AD.ADCSR.BIT.TRGE = 0x0;

   // Disable external trigger
   S12AD.ADCSR.BIT.EXTRG = 0x0;

   // Scan End Interrupt Enable bit. Set to 1 to enable interrupt.
   S12AD.ADCSR.BIT.ADIE = 0x1;

   // A/D data alignment right
   S12AD.ADCER.BIT.ADRFMT = 0x0;

   // Scan Mode Select - Continuous Scan
   S12AD.ADCSR.BIT.ADCS = 0x2;

   // Enable channels for conversion
   ADC_TABLE(EXPAND_AS_ENABLE_CHANNELS_FOR_CONVERSION)

   // Enable interrupts
   IEN(S12AD, S12ADI0) = 0;
   IR(S12AD, S12ADI0) = 0;
   IPR(S12AD, S12ADI0) = E_IRQ_PRIORITY_6;
   IEN(S12AD, S12ADI0) = 1;

   // Start ADC
   S12AD.ADCSR.BIT.ADST = 0x1;
   */
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   IGNORE(channel);
   return 0;
   /*
   AdcCounts_t *result = (uint16_t *)&S12AD.ADDR0;
   AdcCounts_t returnValue = 0;

   if(channel < HardwareChannelCount)
   {
      returnValue = *(result + channel);
   }
   return returnValue;
   */
}

/*
void S12AD_S12ADI0(void) __attribute__((interrupt));
void S12AD_S12ADI0(void)
{
}
*/

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
   ConfigureAdcGroupA();
}
