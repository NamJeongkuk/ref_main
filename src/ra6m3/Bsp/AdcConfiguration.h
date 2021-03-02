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
   ENTRY(Erd_BspAdc_SomeAnalogInput,    00, 00, 0) \
   ENTRY(Erd_BspAdc_AnotherAnalogInput, 00, 01, 1)

// clang-format on

#endif
