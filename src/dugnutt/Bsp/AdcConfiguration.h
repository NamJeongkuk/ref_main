/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

// clang-format off

#define Port
#define Bit
#define Channel

// Name, port, bit, channel
#define ADC_TABLE(ENTRY)         \
   ENTRY(Erd_BspAdc_SomeAnalogInput,     4, 1, 0) \
   ENTRY(Erd_BspAdc_AnotherAnalogInput,  4, 2, 1)

// clang-format on

#endif
