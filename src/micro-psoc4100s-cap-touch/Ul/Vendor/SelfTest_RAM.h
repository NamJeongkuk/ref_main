/*******************************************************************************
* File Name: SelfTest_RAM.h
* Version 1.0
*
* Description:
*  This file provides constants and parameter values used for SRAM    
*  self tests for PSoC 4
*
* Owner:
*  VASY, TARK
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
* significant injury to the user. The inclusion of Cypress' product in a life*
support systems application implies that the manufacturer assumes all risk of
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*******************************************************************************/

#if !defined(SELFTEST_RAM_H)
    #define SELFTEST_RAM_H

#include "CyLib.h"

/*************************************** 
* Function Prototypes 
***************************************/
	
extern void  March_Test_Stack_Init(uint8);	
extern uint8 March_Test_Stack(void);

extern void  March_Test_Init(uint8);
extern uint8 March_Test_SRAM(void);


void SelfTests_Init_SRAM_Checkerboard(void);
uint8 SelfTests_SRAM_Checkerboard(uint8 WordsToTest);

void SelfTests_Init_March_SRAM_Test(uint8);
uint8 SelfTests_SRAM_March(void);   

void SelfTests_Init_March_Stack_Test(uint8);
uint8 SelfTests_Stack_March(void);

/***************************************
* Initial Parameter Constants 
***************************************/


#define CHECKERBOARD_PATTERN_55     0x55u
#define CHECKERBOARD_PATTERN_AA     0xAAu

/* This is a duplicate define of TEST_BLOCK_SIZE defined in asm files */
/* SelfTest_SRAM_March.s */
#define TEST_BLOCK_SIZE                        (0x0400u) 

/* Number of blocks on which SRAM is divided */
#define NUMBERS_OF_TEST_CYCLES                 (uint16)(CYDEV_SRAM_SIZE / TEST_BLOCK_SIZE)

#endif /* SELFTEST_RAM_H */
