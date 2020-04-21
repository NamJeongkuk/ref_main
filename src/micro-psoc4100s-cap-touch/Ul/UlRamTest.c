/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include <stdbool.h>
#include "SelfTest_RAM.h"
#include "SelfTest_Stack.h"
#include "utils.h"
#include "TestConfiguration.h"
#include "UlRamTest.h"
#include "Reset.h"

#define uassert(condition) \
   if(!(condition)) Reset()

void UlRamTest_Init(void)
{
   SelfTests_Init_March_Stack_Test(0);
   SelfTests_Init_Stack_Test();
   SelfTests_Init_March_SRAM_Test(0);
}

void UlRamTest_Run(void)
{
   uint8_t runtimeSramMarchTest_Status = SelfTests_SRAM_March();
   uint8_t runtimeStackMarchTest_Status = SelfTests_Stack_March();
   uint8_t runtimeStackCheck_Status = SelfTests_Stack_Check();

   uassert(ERROR_STATUS != runtimeSramMarchTest_Status);
   if(PASS_COMPLETE_STATUS == runtimeSramMarchTest_Status)
   {
      SelfTests_Init_March_SRAM_Test(0);
   }

   uassert(ERROR_STATUS != runtimeStackMarchTest_Status);
   if(PASS_COMPLETE_STATUS == runtimeStackMarchTest_Status)
   {
      SelfTests_Init_March_Stack_Test(0);
   }

   uassert(ERROR_STATUS != runtimeStackCheck_Status);
   if(OK_STATUS == runtimeStackCheck_Status)
   {
      SelfTests_Init_Stack_Test();
   }
}
