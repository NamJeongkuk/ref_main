/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "BootLoader.h"
#include "stm8s_flash.h"

#define ModeAddress ((volatile uint8_t *)FLASH_DATA_END_PHYSICAL_ADDRESS)

inline void Unlock(void)
{
   FLASH->DUKR = FLASH_RASS_KEY2;
   FLASH->DUKR = FLASH_RASS_KEY1;
   while(!(FLASH->IAPSR & FLASH_FLAG_DUL))
   {
   }
}

void BootLoader_JumpToBootLoader(void)
{
   Unlock();
   *((volatile uint8_t *)ModeAddress) = 0xFF;
   while(1)
   {
   }
}
