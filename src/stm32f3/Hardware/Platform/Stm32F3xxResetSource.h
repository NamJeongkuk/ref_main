/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef STM32F3XXRESETSOURCE_H
#define STM32F3XXRESETSOURCE_H

#include <stdint.h>

enum
{
   Stm32F3xxResetSource_Unknown,
   Stm32F3xxResetSource_LowPower,
   Stm32F3xxResetSource_WindowWatchdog,
   Stm32F3xxResetSource_IndependentWindowWatchdog,
   Stm32F3xxResetSource_Software,
   Stm32F3xxResetSource_PowerOn,
   Stm32F3xxResetSource_NrstPin,
   Stm32F3xxResetSource_OptionByteLoader
};
typedef uint8_t Stm32F3xxResetSource_t;

Stm32F3xxResetSource_t Stm32F3xxResetSource_GetResetSource(void);

#endif
