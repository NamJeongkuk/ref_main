/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

#define Port
#define Pin
#define Channel

// clang-format off
// Name, port, pin, channel
#define ADC_TABLE(ENTRY)         \
   ENTRY(Erd_BspAdc_SomeAnalogInput,    0, 0, 0) \
   ENTRY(Erd_BspAdc_AnotherAnalogInput, 0, 0, 1)

// clang-format on

#endif
