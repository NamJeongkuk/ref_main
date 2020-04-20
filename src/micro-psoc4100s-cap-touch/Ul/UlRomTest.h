/*!
 * @file
 * @brief Runs periodic ROM test for UL60730.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ULROMTEST_H
#define ULROMTEST_H

#include <stdint.h>
#include <stdbool.h>
#include "ImageHeader.h"

/*!
 * Runs the UL ROM test
 * @param imageHeader
 * @param bytesToCrc
 * @param errorEnabled Error should be disabled when debugging
 */
void UlRomTest_Init(
   const ImageHeader_t *imageHeader,
   uint16_t bytesToCrc,
   bool errorEnabled);

/*!
 * Runs the UL ROM test
 */
void UlRomTest_Run(void);

#endif
