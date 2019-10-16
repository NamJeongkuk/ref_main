/*!
 * @file
 * @brief Implementation of RX200 series hardware-based CRC16 calculator.
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#include <stdint.h>
#include "Crc16Calculator_Stm32.h"
#include "stm32f3xx.h"
#include "utils.h"
#include "uassert.h"

// Generating Polynomial Switching register masks
#define GPS_NO_CALCULATION ((uint8_t)0x00)
#define GPS_CRC8_COMPUTE ((uint8_t)0x01)
#define GPS_CRC16_ANSI_COMPUTE ((uint8_t)0x02)
#define GPS_CRC16_CCITT_COMPUTE ((uint8_t)0x03)

// LMS register - CRC Calculation Switching
#define LMS_PERFORM_LSB_FIRST_COMM ((uint8_t)0x0)
#define LMS_PERFORM_MSB_FIRST_COMM ((uint8_t)0x1)

static I_ContextProtector_t *contextProtector;
static CRC_HandleTypeDef crcPeripheral;

static Crc16_t ProcessString(I_Crc16Calculator_t *instance, Crc16_t seed, const void *start, size_t count)
{
   IGNORE_ARG(instance);
   REINTERPRET(current, start, uint8_t *);
   Crc16_t crc;

   ContextProtector_Protect(contextProtector);
   crc = HAL_CRC_Calculate(&crcPeripheral, (uint32_t *)start, count);
   ContextProtector_Unprotect(contextProtector);

   return crc;
}

static const I_Crc16Calculator_Api_t api =
   { ProcessString };

static I_Crc16Calculator_t calculator =
   {
      &api
   };

I_Crc16Calculator_t *Crc16Calculator_Stm32_Init(I_ContextProtector_t *contextProtector)
{
   uassert(contextProtector);
   contextProtector = contextProtector;

   crcPeripheral.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_DISABLE;
   crcPeripheral.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_DISABLE;
   crcPeripheral.Init.GeneratingPolynomial = 0x8810;
   crcPeripheral.Init.CRCLength = CRC_POLYLENGTH_16B;
   crcPeripheral.Init.InitValue = 0x1021;
   crcPeripheral.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
   crcPeripheral.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;

   HAL_CRC_Init(&crcPeripheral);

   return &calculator;
}
