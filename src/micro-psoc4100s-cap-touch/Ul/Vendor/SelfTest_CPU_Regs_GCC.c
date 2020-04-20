/*******************************************************************************
 * File Name: SelfTest_CPU_Regs.c
 * Version 1.0
 *
 * Description:
 *  This file provides the source code for CPU register self tests for
 *  PSoC 4
 *
 * Owner:
 *  VASY
 *
 * Related Document:
 *  AN89056: PSoC 4 - IEC 60730 Class B and IEC 61508 Safety Software Library
 *
 * Hardware Dependency:
 *  CY8C40XX, CY8C41XX and CY8C42XX Devices
 *
 * Note:
 *
 ********************************************************************************
 * Copyright (2014), Cypress Semiconductor Corporation.
 ********************************************************************************
 * This software is owned by Cypress Semiconductor Corporation (Cypress) and is
 * protected by and subject to worldwide patent protection (United States and
 * foreign), United States copyright laws and international treaty provisions.
 * Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
 * license to copy, use, modify, create derivative works of, and compile the
 * Cypress Source Code and derivative works for the sole purpose of creating
 * custom software in support of licensee product to be used only in conjunction
 * with a Cypress integrated circuit as specified in the applicable agreement.
 * Any reproduction, modification, translation, compilation, or representation of
 * this software except as specified above is prohibited without the express
 * written permission of Cypress.
 *
 * Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH
 * REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * Cypress reserves the right to make changes without further notice to the
 * materials described herein. Cypress does not assume any liability arising out
 * of the application or use of any product or circuit described herein. Cypress
 * does not authorize its products for use as critical components in life-support
 * systems where a malfunction or failure may reasonably be expected to result in
 * significant injury to the user. The inclusion of CypressвЂ™ product in a life*
 support systems application implies that the manufacturer assumes all risk of
 * such use and in doing so indemnifies Cypress against all charges. Use may be
 * limited by and subject to the applicable Cypress software license agreement.
 *******************************************************************************/

#include <project.h>
#include "SelfTest_CPU_Regs.h"
#include "TestConfiguration.h"

/*****************************************************************************
 * Function Name: SelfTest_CPU_Regs
 ******************************************************************************
 * Summary:
 *  This function performs checkerboard test for all CPU registers for PSoC 4
 *  except PC register.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  0 - No error
 *  Not 0 - Error detected
 *
 * Note:
 *  This API is C compiler dependent.
 *
 *****************************************************************************/

#if defined(__GNUC__)
__attribute__((naked))  uint8 SelfTest_CPU_Regs(void) /* declaration of function if PSoC 4 GCC compiler */
{
   int ret;

   __ASM volatile
   (
      "PUSH  {R1-R7, LR}          \n\t"

      /* Test SP register */
      /*   Note: SP ignores writes to the lower two bits,
       so they are intentionally set to 0 */

      /* Save SP to R1 */
      "MOV   R1, SP               \n\t"

      /* Load SP with 0xAAAAAAA8 */
      "LDR   R0, =0xAAAAAAA8      \n\t"
      "MOV   SP, R0               \n\t"
      "CMP   SP, R0               \n\t"

      /* Restore SP and return failure */
      "MOV   SP, R1			    \n\t"
      "BNE   __test_stack_fail    \n\t"

      /* Load SP with 0x55555554 */
      "LDR   R0, =0x55555554      \n\t"
      "MOV   SP, R0               \n\t"
      "CMP   SP, R0               \n\t"

      /* Restore SP and return failure */
      "MOV   SP, R1			    \n\t"
      "BNE   __test_stack_fail    \n\t"

      "B     __test_stack_ok		\n\t"

      "__test_stack_fail:			\n\t"
      "B    __test_asm_fail		\n\t"

      "__test_stack_ok:			\n\t"

      /* CPU_REGISTER_TEST_HI("R8") */
      "MOV   R1, R8				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R8, R0				\n\t"
      "CMP   R8, R0				\n\t"
      "MOV   R8, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R8, R0				\n\t"
      "CMP   R8, R0				\n\t"
      "MOV   R8, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_HI("R9") */
      "MOV   R1, R9				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R9, R0				\n\t"
      "CMP   R9, R0				\n\t"
      "MOV   R9, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R9, R0				\n\t"
      "CMP   R9, R0				\n\t"
      "MOV   R9, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_HI("R10") */
      "MOV   R1, R10				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R10, R0				\n\t"
      "CMP   R10, R0				\n\t"
      "MOV   R10, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R10, R0				\n\t"
      "CMP   R10, R0				\n\t"
      "MOV   R10, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_HI("R11") */
      "MOV   R1, R11				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R11, R0				\n\t"
      "CMP   R11, R0				\n\t"
      "MOV   R11, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R11, R0				\n\t"
      "CMP   R11, R0				\n\t"
      "MOV   R11, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_HI("R12") */
      "MOV   R1, R12				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R12, R0				\n\t"
      "CMP   R12, R0				\n\t"
      "MOV   R12, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R12, R0				\n\t"
      "CMP   R12, R0				\n\t"
      "MOV   R12, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_HI("LR") */
      "MOV   R1, LR				\n\t"
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   LR, R0				\n\t"
      "CMP   LR, R0				\n\t"
      "MOV   LR, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   LR, R0				\n\t"
      "CMP   LR, R0				\n\t"
      "MOV   LR, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_R0("R0") */
      "LDR   R1, =0xAAAAAAAA		\n\t"
      "MOV   R0, R1				\n\t"
      "CMP   R0, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R1, =0x55555555		\n\t"
      "MOV   R0, R1				\n\t"
      "CMP   R0, R1				\n\t"
      "BNE   __test_asm_fail		\n\t"

#if (ERROR_IN_CPU_REGISTERS)

      /* CPU_REGISTER_TEST_LO_ERROR("R1") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R1, R0				\n\t"
      "ADD   R1, R1, #0x01		\n\t"
      "CMP   R1, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R1, R0				\n\t"
      "CMP   R1, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"
#else

      /*CPU_REGISTER_TEST_LO("R1") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R1, R0				\n\t"
      "CMP   R1, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R1, R0				\n\t"
      "CMP   R1, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"
#endif

      /* CPU_REGISTER_TEST_LO("R2") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R2, R0				\n\t"
      "CMP   R2, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R2, R0				\n\t"
      "CMP   R2, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_LO("R3") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R3, R0				\n\t"
      "CMP   R3, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R3, R0				\n\t"
      "CMP   R3, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_LO("R4") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R4, R0				\n\t"
      "CMP   R4, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R4, R0				\n\t"
      "CMP   R4, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_LO("R5") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R5, R0				\n\t"
      "CMP   R5, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R5, R0				\n\t"
      "CMP   R5, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_LO("R6") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R6, R0				\n\t"
      "CMP   R6, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R6, R0				\n\t"
      "CMP   R6, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* CPU_REGISTER_TEST_LO("R7") */
      "LDR   R0, =0xAAAAAAAA		\n\t"
      "MOV   R7, R0				\n\t"
      "CMP   R7, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      "LDR   R0, =0x55555555		\n\t"
      "MOV   R7, R0				\n\t"
      "CMP   R7, R0				\n\t"
      "BNE   __test_asm_fail		\n\t"

      /* Return success */
      "MOVS   R0, #0x00			\n\t"
      "MOVS	%[result], R0		\n\t"
      "POP   {R1-R7, PC}			\n\t"

      /* Return failure */
      "__test_asm_fail:			\n\t"
      "MOVS   R0, #0x01			\n\t"
      "MOVS	%[result], R0		\n\t"
      "POP   {R1-R7, PC}			\n\t"
      : [result] "=r" (ret)
   );

   return ret;
}
#endif /* End (__GNUC__) */

/* [] END OF FILE */

