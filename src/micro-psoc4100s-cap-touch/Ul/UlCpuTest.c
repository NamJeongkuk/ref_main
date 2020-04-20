/*!
 * @file
 * @brief Implement test CPU register module.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "SelfTest_CPU.h"
#include "utils.h"
#include "TestConfiguration.h"
#include "UlCpuTest.h"
#include "Reset.h"

#define uassert(condition) \
   if(!condition) Reset()

void UlCpuTest_Init(void)
{
}

void UlCpuTest_Run(void)
{
   uassert(ERROR_STATUS != SelfTest_CPU_Registers());
   uassert(ERROR_STATUS != SelfTest_PC());
}
