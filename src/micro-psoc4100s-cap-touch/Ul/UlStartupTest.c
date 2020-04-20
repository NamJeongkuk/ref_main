/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "UlStartupTest.h"
#include "SelfTest_CPU.h"
#include "SelfTest_RAM.h"
#include "SelfTest_Stack.h"
#include "TestConfiguration.h"
#include "Reset.h"

#define uassert(condition) \
   if(!condition) Reset()

static void RunCpuRegisterTest(void)
{
   uassert(ERROR_STATUS != SelfTest_CPU_Registers());
   uassert(ERROR_STATUS != SelfTest_PC());
}

static void RunPowerOnRamTest(void)
{
   uint8_t ramTestState = OK_STATUS;

   SelfTests_Init_March_SRAM_Test(0);
   SelfTests_Init_March_Stack_Test(0);
   SelfTests_Init_Stack_Test();

   while(1)
   {
      ramTestState = SelfTests_SRAM_March();

      uassert(ERROR_STATUS != ramTestState);

      if(PASS_COMPLETE_STATUS == ramTestState)
      {
         SelfTests_Init_March_SRAM_Test(0);
         break;
      }
   }

   while(1)
   {
      ramTestState = SelfTests_Stack_March();

      uassert(ERROR_STATUS != ramTestState);

      if(PASS_COMPLETE_STATUS == ramTestState)
      {
         SelfTests_Init_March_Stack_Test(0);
         break;
      }
   }

   uassert(ERROR_STATUS != SelfTests_Stack_Check());
}

void UlStartupTest_Run(void)
{
   RunCpuRegisterTest();
   RunPowerOnRamTest();
}
