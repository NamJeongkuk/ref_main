/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "CrcCalculator_Ra.h"
#include "utils.h"
#include "r_crc.h"

static const crc_cfg_t crcConfig = {
   .polynomial = CRC_POLYNOMIAL_CRC_CCITT,
   .bit_order = CRC_BIT_ORDER_LMS_MSB,
   .snoop_address = CRC_SNOOP_ADDRESS_NONE,
   .p_extend = NULL
};

static crc_instance_ctrl_t crcControl;

static I_ContextProtector_t *contextProtector;

static Crc16_t ProcessString(I_Crc16Calculator_t *instance, Crc16_t seed, const void *start, size_t count)
{
   IGNORE_ARG(instance);
   uint32_t result;

   crc_input_t crcInput = {
      .num_bytes = count,
      .crc_seed = seed,
      .p_input_buffer = (void *)start
   };

   ContextProtector_Protect(contextProtector);
   R_CRC_Calculate(&crcControl, &crcInput, &result);
   ContextProtector_Unprotect(contextProtector);

   return (Crc16_t)result;
}

static const I_Crc16Calculator_Api_t api = { ProcessString };
static I_Crc16Calculator_t calculator = { &api };

I_Crc16Calculator_t *Crc16Calculator_Ra_Init(I_ContextProtector_t *_contextProtector)
{
   contextProtector = _contextProtector;
   R_CRC_Open(&crcControl, &crcConfig);

   return &calculator;
}
