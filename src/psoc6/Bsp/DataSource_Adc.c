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
#include "Gpio_Psoc6.h"
#include "utils.h"

#define DEFAULT_CONFIG              \
   CY_SAR_CHAN_SINGLE_ENDED |       \
      CY_SAR_CHAN_SAMPLE_TIME_0 |   \
      CY_SAR_POS_PORT_ADDR_SARMUX | \
      CY_SAR_CHAN_POS_PIN_ADDR_2 |  \
      CY_SAR_CHAN_AVG_DISABLE

#define CTRL                        \
   CY_SAR_VREF_PWR_100 |            \
      CY_SAR_VREF_SEL_VDDA_DIV_2 |  \
      CY_SAR_BYPASS_CAP_ENABLE |    \
      CY_SAR_NEG_SEL_VREF |         \
      CY_SAR_CTRL_NEGVREF_HW |      \
      CY_SAR_CTRL_COMP_DLY_2P5 |    \
      CY_SAR_COMP_PWR_100 |         \
      CY_SAR_DEEPSLEEP_SARMUX_OFF | \
      CY_SAR_SARSEQ_SWITCH_ENABLE

#define SAMPLE_CTRL                      \
   CY_SAR_RIGHT_ALIGN |                  \
      CY_SAR_SINGLE_ENDED_UNSIGNED |     \
      CY_SAR_DIFFERENTIAL_SIGNED |       \
      CY_SAR_AVG_CNT_2 |                 \
      CY_SAR_AVG_MODE_SEQUENTIAL_FIXED | \
      CY_SAR_TRIGGER_MODE_FW_ONLY

#define SAMPLE_TIME (4 << CY_SAR_SAMPLE_TIME0_SHIFT) | (4 << CY_SAR_SAMPLE_TIME1_SHIFT)
#define RANGE_THRES (0x0uL << CY_SAR_RANGE_LOW_SHIFT) | (0xC00uL << CY_SAR_RANGE_HIGH_SHIFT)
#define INTR_MASK CY_SAR_INTR_EOS | CY_SAR_INTR_OVERFLOW
#define MUX_SWITCH CY_SAR_MUX_FW_P2_VPLUS | CY_SAR_MUX_FW_P0_VPLUS | CY_SAR_MUX_FW_P1_VMINUS
#define MUX_SWITCH_SQ_CTRL CY_SAR_MUX_SQ_CTRL_P0 | CY_SAR_MUX_SQ_CTRL_P1 | CY_SAR_MUX_SQ_CTRL_P2

static cy_stc_sar_config_t config = {
   (uint32_t)(CTRL),
   (uint32_t)(SAMPLE_CTRL),
   SAMPLE_TIME,
   SAMPLE_TIME,
   RANGE_THRES,
   CY_SAR_RANGE_COND_OUTSIDE,
   0,
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
   INTR_MASK,
   0u,
   0u,
   MUX_SWITCH,
   MUX_SWITCH_SQ_CTRL,
   true,
   1650u,
   CY_SAR_CLK_PERI,
   (void *)NULL,
   false,
   false,
   false
};

#define ERD_IS_IN_RANGE(erd) (IN_RANGE(Erd_BspAdc_Start + 1, erd, Erd_BspAdc_End - 1))

#define EXPAND_AS_CONFIGURE_PINS(name, port, pin, channel)                 \
   {                                                                       \
      static const cy_stc_gpio_pin_config_t config = {                     \
         .outVal = 1,                                                      \
         .driveMode = CY_GPIO_DM_ANALOG,                                   \
         .hsiom = CONCAT(P, CONCAT(port, CONCAT(_, CONCAT(pin, _AMUXA)))), \
         .intEdge = CY_GPIO_INTR_DISABLE,                                  \
         .intMask = 0UL,                                                   \
         .vtrip = CY_GPIO_VTRIP_CMOS,                                      \
         .slewRate = CY_GPIO_SLEW_FAST,                                    \
         .driveSel = CY_GPIO_DRIVE_1_2,                                    \
         .vregEn = 0UL,                                                    \
         .ibufMode = 0UL,                                                  \
         .vtripSel = 0UL,                                                  \
         .vrefSel = 0UL,                                                   \
         .vohSel = 0UL,                                                    \
      };                                                                   \
      Gpio_Psoc6_ConfigurePin(port, pin, &config);                         \
   }

#define EXPAND_AS_ERD_ADC_CHANNEL_PAIRS(name, port, pin, channel) \
   {                                                              \
      name, channel                                               \
   },

enum
{
   HardwareChannelCount = CY_SAR_NUM_CHANNELS
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

static void ConfigurePins(void)
{
   ADC_TABLE(EXPAND_AS_CONFIGURE_PINS)
}

static void ConfigureAdc(void)
{
   for(uint8_t i = 0; i < NUM_ELEMENTS(erdAdcChannelPairs); i++)
   {
      config.chanConfig[erdAdcChannelPairs[i].channel] = DEFAULT_CONFIG;
      config.chanEn |= erdAdcChannelPairs[i].channel;
   }

   cy_en_sar_status_t status = Cy_SAR_Init(SAR, &config);
   uassert(CY_SAR_SUCCESS == status);

   Cy_SAR_Enable(SAR);
   Cy_SAR_StartConvert(SAR, CY_SAR_START_CONVERT_CONTINUOUS);
}

static AdcCounts_t ReadAdcChannel(uint8_t channel)
{
   AdcCounts_t result = 0;

   if(channel < HardwareChannelCount)
   {
      result = Cy_SAR_GetResult16(SAR, channel);
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
