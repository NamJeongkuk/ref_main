/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CRCCALCULATOR_RA_H
#define CRCCALCULATOR_RA_H

#include "I_Crc16Calculator.h"
#include "I_ContextProtector.h"

/*!
 * @param contextProtector
 * @return
 */
I_Crc16Calculator_t *Crc16Calculator_Ra_Init(I_ContextProtector_t *_contextProtector);

#endif
