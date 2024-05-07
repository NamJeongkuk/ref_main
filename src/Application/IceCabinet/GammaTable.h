/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GAMMATABLE_H
#define GAMMATABLE_H

#include <stdint.h>

typedef uint8_t Gamma_t;

typedef struct
{
   Gamma_t iceCabinetMedSpeedLimit;
   Gamma_t iceCabinetHighSpeedLimit;
   Gamma_t iceCabinetSuperHighSpeedLimit;
} GammaTable_t;

#endif
