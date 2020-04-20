/* *****************************************************************************
* File Name: SelfTest_Stack.c
* Version 1.0
*
* Description:
*  This file provides the source code to the API for runtime Stack self tests for 
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
* significant injury to the user. The inclusion of Cypress product in a life
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#include "SelfTest_Stack.h"
#include "TestConfiguration.h"
#include "CyLib.h"

/*******************************************************************************
* Function Name: SelfTests_Init_Stack_Test
********************************************************************************
*
* Summary: 
*  This function initializes the upper stack area with 0xAA and 0x55 pattern.
*
* Parameters:
*  None.
*                           
* Return:
*  None.
* 
**********************************************************************************/
void SelfTests_Init_Stack_Test(void)
{
	/* Pointer to the last word in the stack*/
	uint16 *stack = (uint16 *)PSOC_STACK_END;
		
	/* Fill test stack block with predefined pattern */
	for (uint8 i = 0u; i < (STACK_TEST_BLOCK_SIZE / sizeof(uint16)); i++)
	{
		#if (ERROR_IN_STACK)
			*stack++ = STACK_TEST_PATTERN + 1u;
		#else
			*stack++ = STACK_TEST_PATTERN;
		#endif /* End (ERROR_IN_STACK) */
	}
}


/*******************************************************************************
* Function Name: SelfTests_Stack_Check
********************************************************************************
*
* Summary: 
*  This function performs stack self test. It checks upper stack area for 0xAA 
*  and 0x55 pattern.
*
* Parameters:
*  None.
*                           
* Return:
*  Result of test:  "0" - pass test; "1" - fail test.
* 
**********************************************************************************/    
uint8 SelfTests_Stack_Check(void)
{
	uint8 ret = OK_STATUS;
		
	/* Pointer to the last word in the stack */
	uint16 *stack = (uint16 *)PSOC_STACK_END;
		
	/* Check test stack block for pattern and return error if no pattern found */
	for (uint8 i = 0u; i < (STACK_TEST_BLOCK_SIZE / sizeof(uint16)); i++)
	{
		if (*stack++ != STACK_TEST_PATTERN)
		{
			ret = ERROR_STATUS;
			break;
		}
	}
		
	return ret;
}


/* [] END OF FILE */


