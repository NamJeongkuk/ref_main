/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

#define ON_CHIP (uint8_t)(0x00)

// clang-format off

// Name, port, pin, channel
#define ADC_TABLE(ENTRY)         \
   ENTRY(SomeAnalogInput,        GPIOA,      GPIO_PIN_6, ADC_CHANNEL_3) \
   ENTRY(AnotherAnalogInput,     GPIOA,      GPIO_PIN_7, ADC_CHANNEL_4) \
   ENTRY(TemperatureSensor,      ON_CHIP,    ON_CHIP,    ADC_CHANNEL_TEMPSENSOR)

// clang-format on

#endif
