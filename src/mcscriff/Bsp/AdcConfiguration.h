/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

// clang-format off

// Name, port, pin, channel
#define ADC_TABLE(ENTRY)         \
   ENTRY(SomeAnalogInput,     GPIOA, GPIO_PIN_6, 3) \
   ENTRY(AnotherAnalogInput,  GPIOA, GPIO_PIN_7, 4)

// clang-format on

#endif
