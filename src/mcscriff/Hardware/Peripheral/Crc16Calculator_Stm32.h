/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef CRC16CALCULATOR_STM32_H
#define CRC16CALCULATOR_STM32_H

#include "I_Crc16Calculator.h"
#include "I_ContextProtector.h"

/*!
 * @param contextProtector
 * @return
 */
I_Crc16Calculator_t *Crc16Calculator_Stm32_Init(I_ContextProtector_t *contextProtector);

#endif
