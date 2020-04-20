/* *****************************************************************************
 * File Name: SelfTest_RAM.c
 * Version 1.0
 *
 * Description:
 *  This file provides the source code to the API for runtime SRAM self tests for
 *  PSoC 4
 *
 * Owner:
 *  VASY, TARK
 *
 * Related Document:
 *  AN89056: PSoC 4 - IEC 60730 Class B and IEC 61508 Safety Software Library
 *
 * Hardware Dependency:
 *  CY8C40XX, CY8C41XX and CY8C42XX Devices
 *
 * Note:
 *
 * Update :
 * --------
 * Modified to CyEnterCriticalSection/CyExitCriticalSection instead of
 *      CyGlobalIntEnable/CyGlobalIntDisable - Vikram
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
 * significant injury to the user. The inclusion of Cypress product in a life
 * support systems application implies that the manufacturer assumes all risk of
 * such use and in doing so indemnifies Cypress against all charges. Use may be
 * limited by and subject to the applicable Cypress software license agreement.
 *******************************************************************************/

#include "SelfTest_RAM.h"
#include "TestConfiguration.h"
#include "CyLib.h"

/*******************************************************************************
 * Function Name: SelfTests_Init_SRAM_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  shift - set shift from start address for March RAM test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_March_SRAM_Test(uint8 shift)
{
   /* Set base address for March RAM test */
   March_Test_Init(shift);
}

/*******************************************************************************
 * Function Name: SelfTests_SRAM_March
 ********************************************************************************
 *
 * Summary:
 *  This function perform SRAM self test using March method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "1" - fail test; "2" - still testing; "3" - Test complete OK
 *
 **********************************************************************************/
uint8 SelfTests_SRAM_March(void)
{
   uint8 test_Status;

   uint8_t savedInterruptStatus = CyEnterCriticalSection();
   {
      /* Perform March test */
      test_Status = March_Test_SRAM();
   }
   CyExitCriticalSection(savedInterruptStatus);

   return test_Status;
}

/*******************************************************************************
 * Function Name: SelfTests_Init_SRAM_Test
 ********************************************************************************
 *
 * Summary:
 *  This function initializes the SRAM base address.
 *
 * Parameters:
 *  shift - set shift from start address for March Stack test
 *
 * Return:
 *  None.
 *
 **********************************************************************************/
void SelfTests_Init_March_Stack_Test(uint8 shift)
{
   /* Set base address for March Stack test */
   March_Test_Stack_Init(shift);
}

/*******************************************************************************
 * Function Name: SelfTests_Stack_March
 ********************************************************************************
 *
 * Summary:
 *  This function perform Stack RAM self test using March method.
 *
 * Parameters:
 *  None.
 *
 * Return:
 *  Result of test:  "0" - pass test; "1" - fail test.
 *
 **********************************************************************************/
uint8 SelfTests_Stack_March(void)
{
   uint8 test_Status;

   uint8_t savedInterruptStatus = CyEnterCriticalSection();
   {
      /* Perform March test */
      test_Status = March_Test_Stack();
   }
   CyExitCriticalSection(savedInterruptStatus);

   return test_Status;
}

/* [] END OF FILE */
