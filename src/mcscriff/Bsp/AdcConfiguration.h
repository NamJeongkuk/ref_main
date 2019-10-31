/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

// clang-format off

// Name, port, pin, channel, rank
#define ADC_TABLE(ENTRY)         \
   ENTRY(SomeAnalogInput,     GPIOA, GPIO_PIN_6, ADC_CHANNEL_3, ADC_REGULAR_RANK_1) \
   ENTRY(AnotherAnalogInput,  GPIOA, GPIO_PIN_7, ADC_CHANNEL_4, ADC_REGULAR_RANK_2)

// clang-format on

#endif
