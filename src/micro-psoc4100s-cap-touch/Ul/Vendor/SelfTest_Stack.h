/*******************************************************************************
* File Name: SelfTest_Stack.h
*  Version 1.0
*
* Description:
*  This file provides constants and parameter values used for Stack    
*  self tests for PSoC 4
*
* Owner:
*  VASY
* 
* Related Document: 
*  AN89056: PSoC 4 - IEC 60730 Class B and IEC 61508 Safety Software Library
*
* Hardware Dependency:
*  CY8C41XX and CY8C42XX Devices
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
* significant injury to the user. The inclusion of Cypress' product in a life
* support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(SELFTEST_STACK_H)
    #define SELFTEST_STACK_H

#include "CyLib.h"

/*************************************** 
* Function Prototypes 
***************************************/

void SelfTests_Init_Stack_Test(void);
uint8 SelfTests_Stack_Check(void);


/***************************************
* Initial Parameter Constants 
***************************************/

/* Stack test parameters */
#define STACK_TEST_PATTERN        0x55AAu

/* Block size to be tested. Should be EVEN*/
#define STACK_TEST_BLOCK_SIZE     0x08u

/* PSoC memory parameters */
#define PSOC_SRAM_SIZE            CYDEV_SRAM_SIZE
#define PSOC_SRAM_BASE            CYDEV_SRAM_BASE

/* PSoC stack parameters */
#define PSOC_STACK_BASE			  (CYDEV_SRAM_BASE + CYDEV_SRAM_SIZE)
#define PSOC_STACK_SIZE			  (CYDEV_STACK_SIZE)
#define PSOC_STACK_END			  ((uint32)(PSOC_STACK_BASE - PSOC_STACK_SIZE))

#endif /* SELFTEST_STACK_H */


/* END OF FILE */

