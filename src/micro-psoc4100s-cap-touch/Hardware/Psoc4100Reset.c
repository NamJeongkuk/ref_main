/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdint.h>
#include "Psoc4100Reset.h"
#include "cydevice_trm.h"

#define AIRCR_REGISTER (*(volatile uint32_t *)CYREG_CM0P_AIRCR)

void Psoc4100Reset(void)
{
   AIRCR_REGISTER = (0x5FA << CYFLD_CM0P_VECTKEY__OFFSET) | (1 << CYFLD_CM0P_SYSRESETREQ__OFFSET);
}
