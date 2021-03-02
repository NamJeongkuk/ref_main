/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <string.h>
#include "DataSource_Adc.h"
#include "I_Adc.h"
#include "uassert.h"
#include "Event_Null.h"
#include "ConstArrayMap_LinearSearch.h"
#include "Gpio_Ra.h"
#include "utils.h"
#include "r_adc.h"

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End - 1))

#define EXPAND_AS_CONFIGURE_PINS(name, port, pin, channel) \
   Gpio_Ra_ConfigurePin(BSP_IO_PORT_##port##_PIN_##pin, IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_CAC_AD);

#define EXPAND_AS_SET_SCAN_MASK_BIT(name, port, pin, channel) \
   | ADC_MASK_CHANNEL_##channel

#define EXPAND_AS_ERD_ADC_CHANNEL_PAIRS(name, port, pin, channel) \
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

static adc_instance_ctrl_t adcControl;

static const adc_extended_cfg_t extendedConfiguration = {
   .add_average_count = ADC_ADD_OFF,
   .clearing = ADC_CLEAR_AFTER_READ_ON,
   .trigger_group_b = ADC_TRIGGER_SYNC_ELC,
   .double_trigger_mode = ADC_DOUBLE_TRIGGER_DISABLED,
   .adc_vref_control = ADC_VREF_CONTROL_VREFH
};

static const adc_cfg_t configuration = {
   .unit = 0,
   .mode = ADC_MODE_CONTINUOUS_SCAN,
   .resolution = ADC_RESOLUTION_12_BIT,
   .alignment = ADC_ALIGNMENT_RIGHT,
   .trigger = ADC_TRIGGER_SOFTWARE,
   .p_callback = NULL,
   .p_context = NULL,
   .p_extend = &extendedConfiguration,
   .scan_end_irq = FSP_INVALID_VECTOR,
   .scan_end_ipl = BSP_IRQ_DISABLED,
   .scan_end_b_irq = FSP_INVALID_VECTOR,
   .scan_end_b_ipl = BSP_IRQ_DISABLED,
};

static adc_channel_cfg_t channelConfiguration = {
   .scan_mask = ADC_MASK_TEMPERATURE,
   .scan_mask_group_b = 0,
   .priority_group_a = ADC_GROUP_A_PRIORITY_OFF,
   .add_mask = 0,
   .sample_hold_mask = 0,
   .sample_hold_states = 24,
};

static void ConfigurePins(void)
{
   ADC_TABLE(EXPAND_AS_CONFIGURE_PINS)
}

static void ConfigureAdc(void)
{
   channelConfiguration.scan_mask |= 0 ADC_TABLE(EXPAND_AS_SET_SCAN_MASK_BIT);

   fsp_err_t result;
   result = R_ADC_Open(&adcControl, &configuration);
   result |= R_ADC_ScanCfg(&adcControl, &channelConfiguration);
   result |= R_ADC_ScanStart(&adcControl);
   uassert(result == FSP_SUCCESS);
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   AdcCounts_t result;

   if(channel < HardwareChannelCount)
   {
      R_ADC_Read(&adcControl, (adc_channel_t)channel, &result);
   }
   return result;
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

   ConfigurePins();
   ConfigureAdc();

   return &instance.interface;
}
