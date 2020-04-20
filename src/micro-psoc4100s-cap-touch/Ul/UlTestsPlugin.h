/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ULTESTSPLUGIN_H
#define ULTESTSPLUGIN_H

#include <stdint.h>
#include <stdbool.h>
#include "TinyTimer.h"
#include "ImageHeader.h"

/*!
 * Plugin for the UL Periodic Checks
 * @param instance
 * @param timerModule
 * @param reset
 * @param crc16Calculator
 * @param applicationHeader
 * @param romCheckErrorEnabled Enables uassert when the application CRC do not match the calculated.
 *    This should be false when debugging.
 * @param testPeriod
 * @param resourceWatchdogTimeout
 * @param bytesToCrcPerRomCheck
 * @param watchdogAction
 */
void UlTestsPlugin_Init(
   TinyTimerModule_t *timerModule,
   const ImageHeader_t *applicationHeader,
   bool romCheckErrorEnabled,
   TinyTimerTicks_t testPeriod,
   TinyTimerTicks_t resourceWatchdogTimeout,
   uint16_t bytesToCrcPerRomCheck);

#endif
