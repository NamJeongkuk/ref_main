/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_Adc.h"
}

#include "DataSource_NonMockTestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(name, port, bit, channel) \
   {                                                                          \
      .erd = name, .sizeOfEntity = sizeof(AdcCounts_t)                        \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
} instance;

static const DataSource_NonMockTestDoubleItem_t adcItems[] = {
   ADC_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

extern "C" I_DataSource_t *DataSource_Adc_Init(void)
{
   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      adcItems,
      NUM_ELEMENTS(adcItems));

   return &instance.dataSourceNonMockTestDouble.interface;
}
