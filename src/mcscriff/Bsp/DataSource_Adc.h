/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_ADC_H
#define DATASOURCE_ADC_H

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "AdcConfiguration.h"
#include "Timer.h"

#define ADC_TABLE_EXPAND_AS_HARDWARE_ERDS(name, port, bit, channel) \
   Erd_BspAdc_##name,

enum
{
   Erd_BspAdc_Start = (BspErdAdcStart - 1),
   ADC_TABLE(ADC_TABLE_EXPAND_AS_HARDWARE_ERDS)
      Erd_BspAdc_End
};

/*!
 * @param timerModule
 * @return
 */
I_DataSource_t *DataSource_Adc_Init(TimerModule_t *timerModule);

#endif
