/*----------------------------------------------------------------------------------*/
/* FILENAME: SelfTest_SRAM_March.s                                                  */
/* Version 1.0                                                                      */
/*                                                                                  */
/* DESCRIPTION:                                                                     */
/*  This file contains functions to perform SRAM self test using March method.      */
/*                                                                                  */
/* Owner:                                                                           */
/*  VASY, TARK                                                                      */
/*                                                                                  */
/* Related Document:                                                                */
/*  AN89056: PSoC 4 - IEC 60730 Class B and IEC 61508 Safety Software Library       */
/*                                                                                  */
/* Hardware Dependency:                                                             */
/*  CY8C40XX, CY8C41XX and CY8C42XX Devices                                         */
/*                                                                                  */
/*-------------------------------------------------------------------------------   */
/* Copyright (2014), Cypress Semiconductor Corporation.                             */
/*-------------------------------------------------------------------------------   */
/* This software is owned by Cypress Semiconductor Corporation (Cypress) and is     */
/* protected by and subject to worldwide patent protection (United States and       */
/* foreign), United States copyright laws and international treaty provisions.      */
/* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable    */
/* license to copy, use, modify, create derivative works of, and compile the        */
/* Cypress Source Code and derivative works for the sole purpose of creating        */
/* custom software in support of licensee product to be used only in conjunction    */
/* with a Cypress integrated circuit as specified in the applicable agreement.      */
/* Any reproduction, modification, translation, compilation, or representation of   */
/* this software except as specified above is prohibited without the express        */
/* written permission of Cypress.                                                   */
/*                                                                                  */
/* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH      */
/* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED              */
/* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.              */
/* Cypress reserves the right to make changes without further notice to the         */
/* materials described herein. Cypress does not assume any liability arising out    */
/* of the application or use of any product or circuit described herein. Cypress    */
/* does not authorize its products for use as critical components in life-support   */
/* systems where a malfunction or failure may reasonably be expected to result in   */
/* significant injury to the user. The inclusion of Cypress' product in a life      */
/* support systems application implies that the manufacturer assumes all risk of    */
/* such use and in doing so indemnifies Cypress against all charges. Use may be     */
/* limited by and subject to the applicable Cypress software license agreement.     */
/*----------------------------------------------------------------------------------*/


.include "cyfittergnu.inc"
.include "cydevicegnu_trm.inc"


.data   

Test_Stack_Addr:
.align
.word  0x00000000 

.syntax unified

.text
.thumb

/* Enable this define to do intentional error during SRAM March test */
.equ ERROR_IN_SRAM_MARCH,  0x00

/* Enable this define to do intentional error during STACK March test */
.equ ERROR_IN_STACK_MARCH, 0x00

/* Set Test Block size for SRAM test */
.equ TEST_BLOCK_SRAM_SIZE,  0x00000001

/* Set Test Block size for Stack test */
.equ TEST_BLOCK_STACK_SIZE, 0x00000001

/* Choose maximum block size for reserved area */
.if (TEST_BLOCK_SRAM_SIZE > TEST_BLOCK_STACK_SIZE)
    .equ TEST_BLOCK_SIZE,	TEST_BLOCK_SRAM_SIZE 
.else
    .equ TEST_BLOCK_SIZE,	TEST_BLOCK_STACK_SIZE 
.endif


.equ MARCH_SRAM_BASE,           (__no_ul_ram_checks_end)
.equ MARCH_STACK_BASE,          (CYDEV_SRAM_BASE + CYDEV_SRAM_SIZE - CYDEV_STACK_SIZE)
.equ MARCH_STACK_END,           (CYDEV_SRAM_BASE + CYDEV_SRAM_SIZE)
.equ MARCH_BUFF_ADDR_START, 	(MARCH_STACK_BASE - TEST_BLOCK_SIZE)    
.equ MARCH_BUFF_ADDR_END,       (MARCH_BUFF_ADDR_START + TEST_BLOCK_SRAM_SIZE)
.equ MARCH_BUFF_ADDR_STACK_END, (MARCH_BUFF_ADDR_START + TEST_BLOCK_STACK_SIZE)


/* Status flags to return function results */
.equ OK_STATUS,   		         0x00
.equ ERROR_STATUS,  	         0x01
.equ PASS_STILL_TESTING_STATUS,  0x02 
.equ PASS_COMPLETE_STATUS,       0x03 


/*----------------------------------------------------------------------------------*/
/* March_Test_Block																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function perform block of RAM self test using March method. 				*/
/*																					*/
/* Parameters:																		*/
/*   r0 - used to set address of first block byte. 									*/
/*   r1 - used to set address of last block byte. 									*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r3, r4, r10																	*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Block(void); */
.global March_Test_Block
.func March_Test_Block, March_Test_Block
.type March_Test_Block, %function
.thumb_func
March_Test_Block:

    mov r10, LR

	/* Save r0 */
	mov r4, r0
	
	/* Write 0x00 to all block of SRAM */
    bl March_Test_Write_0
	mov r0, r4
    
    /* Read byte, compare with 0x00 and write 0xFF to all block of SRAM  */
    bl March_Test_Read0_Write1_Inc
    cmp r7, #0
    bne End0
	mov r0, r4
	
      
    /* Read byte, compare with 0xFF and write 0x00 to all block of SRAM */
    bl March_Test_Read1_Write0_Inc
    cmp r7, #0
    bne End0
	mov r0, r4
    
    /* Read byte, compare with 0x00 to all block of SRAM */  
	bl March_Test_Read_0
    cmp r7, #0
    bne End0
    mov r0, r4
	
		
	/* Save r1*/
	mov r4, r1
	
	/* Read byte, compare with 0x00 and write 0xFF to all block of SRAM */
    bl March_Test_Read0_Write1_Dec
    cmp r7, #0
    bne End0
	mov r1, r4
    
    /* Read byte, compare with 0xFF and write 0x00 to all block of SRAM */ 
    bl March_Test_Read1_Write0_Dec    
    cmp r7, #0
    bne End0
	mov r1, r4

	/* Save r0*/
	mov r4, r0

	/* Read byte, compare with 0x00 to all block of SRAM */  
    bl March_Test_Read_0
    cmp r7, #0
    bne End0  
	mov r0, r4
    
    End0: 

    mov LR, r10
    
    BX lr
    
.endfunc


/*----------------------------------------------------------------------------------*/
/* Copy_Block_To_Buffer																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function save "memory block under test" to reserved location.				*/
/*   Function copies data from address r2 to r0. Size of copying size is (r1 - r0). */
/*																					*/
/* Parameters:																		*/
/*   r0 - high byte of start address for copying. 									*/
/*   r1 - high byte of end address for copying. 							    	*/
/*   r2 - high byte of buffer address.   											*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r2, r3,																	*/
/*----------------------------------------------------------------------------------*/
/* void Copy_Block_To_Buffer(void); */
.global Copy_Block_To_Buffer
.func Copy_Block_To_Buffer, Copy_Block_To_Buffer
.type Copy_Block_To_Buffer, %function
.thumb_func
Copy_Block_To_Buffer:
	
    IRAM_copy0:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Copt byte of SRAM from r3 to reserved area */
        STRB r3, [r2]

        /* Increment address of copied area */
        adds r0, r0, #1

        /* Increment address of reserved area */
        adds r2, r2, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_copy0    

    BX lr    

.endfunc  


/*------------------------------------------------------------------------------------*/
/* Copy_Block_From_Buffer                                                             */
/*------------------------------------------------------------------------------------*/
/*                                                                                    */
/* Summary:                                                                           */
/*   This function restore "memory block under test" from buffer location.            */
/*   Function copies data from address r2 to r0. Size of copying size is (r1 - r0).   */
/*                                                                                    */
/* Parameters:                                                                        */
/*   r0 - high byte of start address for copying.                                     */
/*   r1 - high byte of end address for copying.                                       */
/*   r2 - high byte of buffer address.                                                */
/* Return:                                                                            */
/*   void.                                                                            */
/*                                                                                    */
/* Destroys:                                                                          */
/*   r0, r2, r3                                                                       */
/*------------------------------------------------------------------------------------*/
/* void Copy_Block_From_Buffer(void); */
.global Copy_Block_From_Buffer
.func Copy_Block_From_Buffer, Copy_Block_From_Buffer
.type Copy_Block_From_Buffer, %function
.thumb_func
Copy_Block_From_Buffer:

    IRAM_copy1:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r2]
        
        /* Copt byte of SRAM from r3 to reserved area */
        STRB r3, [r0]

        /* Increment address of copied area */
        adds r0, r0, #1

        /* Increment address of reserved area */
        adds r2, r2, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_copy1  	
	
    BX lr  	
	
.endfunc  
	

/*----------------------------------------------------------------------------------*/
/* March_Test_Write_0																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Write 0x00 to block of SRAM. Address of memory increnets.				    	*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3																			*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Write_0(void); */
.global March_Test_Write_0
.func March_Test_Write_0, March_Test_Write_0
.type March_Test_Write_0, %function
.thumb_func
	
March_Test_Write_0:
        
	movs r3, #0x00
		  		
	IRAM_chk0:
        
		STRB r3, [r0]

		adds r0, r0, #1

		/* Check if it is not end of SRAM block */
		cmp r0, r1
        
		blt IRAM_chk0
		
	/* Check if intentional error should be made for testing */
	.if (ERROR_IN_SRAM_MARCH | ERROR_IN_STACK_MARCH)
        
		/* Set 0x01 for writing to RAM */
		movs r3, #0x01     
		
		/* Set address of last byte in block */
		subs r0, r0, #1
		
		/* Write 0x01 to RAM */
		STRB r3, [r0]
		
		/* Restore original address in r0 */
		adds r0, r0, #1
			
	.endif  		

	BX lr
        
.endfunc   
  

 
/*----------------------------------------------------------------------------------*/
/* March_Test_Write_1																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Write 0xFF to block of SRAM. Address of memory increnets.						*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3																			*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Write_1(void); */
.global March_Test_Write_1
.func March_Test_Write_1, March_Test_Write_1
.type March_Test_Write_1, %function
.thumb_func
March_Test_Write_1:

    movs r3, #0xFF
    IRAM_chk1:
    
        STRB r3, [r0]

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk1

    BX lr
    
.endfunc   


/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Inc														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00 and write 0xFF to block.							*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																	*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Inc(void); */

.global March_Test_Read0_Write1_Inc
.func March_Test_Read0_Write1_Inc, March_Test_Read0_Write1_Inc
.type March_Test_Read0_Write1_Inc, %function
.thumb_func
March_Test_Read0_Write1_Inc:

	IRAM_chk2:
        
        /* Write byte of SRAM to r3 */
    	LDRB r3, [r0]
		
		/* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_0xFF
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0xFF:
        
        movs r3, #0xFF
        STRB r3, [r0]

        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk2

    BX lr
 
.endfunc   
    
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Inc														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0xFF and write 0x00 to block.							*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																		*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Inc(void); */

.global March_Test_Read1_Write0_Inc
.func March_Test_Read1_Write0_Inc, March_Test_Read1_Write0_Inc
.type March_Test_Read1_Write0_Inc, %function
.thumb_func
March_Test_Read1_Write0_Inc:

    IRAM_chk3:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0xFF 
        
        beq Write_0x00
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0x00:
        
        movs r3, #0x00
        STRB r3, [r0]

        /* Increnemt SRAM pointer register */
        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        blt IRAM_chk3

    BX lr
 
.endfunc      
  

/*----------------------------------------------------------------------------------*/
/* March_Test_Read0_Write1_Dec														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00 and write 0xFF to block.							*/
/*   Address of memory decreneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r1, r3, r7																	*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read0_Write1_Dec(void); */

.global March_Test_Read0_Write1_Dec
.func March_Test_Read0_Write1_Dec, March_Test_Read0_Write1_Dec
.type March_Test_Read0_Write1_Dec, %function
.thumb_func
March_Test_Read0_Write1_Dec:

    subs r1, r1, #1

    IRAM_chk5:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r1]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_0xFF_1
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0xFF_1:
        
        movs r3, #0xFF
        STRB r3, [r1]

        /* Decrenemt SRAM pointer register */
        subs r1, r1, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        ble IRAM_chk5

    BX lr
 
.endfunc        
    
    
/*----------------------------------------------------------------------------------*/
/* March_Test_Read1_Write0_Dec														*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0xFF and write 0x00 to block.							*/
/*   Address of memory decreneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r1, r3, r7																	*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read1_Write0_Dec(void); */

.global March_Test_Read1_Write0_Dec
.func March_Test_Read1_Write0_Dec, March_Test_Read1_Write0_Dec
.type March_Test_Read1_Write0_Dec, %function
.thumb_func
March_Test_Read1_Write0_Dec:

    subs r1, r1, #1

    IRAM_chk6:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r1]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0xFF 
        
        beq Write_0x00_1
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 
        
        /* Write 0xFF to byte in SRAM */
        Write_0x00_1:
        
        movs r3, #0
        STRB r3, [r1]

        /* Increnemt SRAM pointer register */
        subs r1, r1, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
        ble IRAM_chk6

    BX lr     
    
.endfunc  


/*----------------------------------------------------------------------------------*/
/* March_Test_Read_0																*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   Check block of SRAM with 0x00.													*/
/*   Address of memory increneted.													*/
/*																					*/
/* Parameters:																		*/
/*   r0 - register used to set address of first test byte 							*/
/*   r1 - register used to set address of last test byte 							*/
/*																					*/
/* Return:																			*/
/*   r7 - Result of test:  "0" - pass test; "1" - fail test.						*/
/*																					*/
/* Destroys:																		*/
/*   r0, r3, r7																		*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Read_0(void); */

.global March_Test_Read_0
.func March_Test_Read_0, March_Test_Read_0
.type March_Test_Read_0, %function
.thumb_func
March_Test_Read_0:

    IRAM_chk4:
    
        /* Write byte of SRAM to r3 */
        LDRB r3, [r0]
        
        /* Compare content of byte in SRAM with 0x00 */
        cmp r3, #0 
        
        beq Write_next_loop
        
            /* return ERROR_STATUS */
            movs r7, #ERROR_STATUS    

            BX lr 

        Write_next_loop:

        adds r0, r0, #1

        /* Check if it is not end of SRAM block */
        cmp r0, r1
    
    blt IRAM_chk4

    BX lr

.endfunc  


/*----------------------------------------------------------------------------------*/
/* March_Test_Init																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*   This function initializes the SRAM base address.								*/
/*																					*/
/* Parameters:																		*/
/*   Function uses register R11 to set start address of tested block				*/
/*																					*/
/* Return:																			*/
/*   void.																			*/
/*																					*/
/* Destroys:																		*/
/*   r0, r11																		*/
/*----------------------------------------------------------------------------------*/
/* void March_Test_Init(void); */
.global March_Test_Init
.func March_Test_Init, March_Test_Init
.type March_Test_Init, %function
.thumb_func
March_Test_Init:

    PUSH  {R1}

	/* Set base address for March checkerboard test */
    LDR r1, =MARCH_SRAM_BASE

	MOV r11, r1
	ADD r11, r0
    
    POP  {R1}	
    
    BX lr
    
.endfunc    


/*----------------------------------------------------------------------------------*/
/* March_Test_SRAM																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*  This function perform block of RAM self test using March method. This function  */
/*  first duplicates data in the area to be tested as this test is destructive.		*/
/*  At the end of this test the original data is restored to its correct location 	*/
/*  in memory. 																		*/
/*																					*/
/* Parameters:																		*/	
/*   Function uses register R11 to set start address of tested block				*/
/*																					*/
/* Return:																			*/
/*   Result of test:  "1" - fail test;												*/
/*	                  "2" - pass, but still testing status							*/
/*                    "3" - pass and complete status								*/
/*																					*/
/* Destroys:																		*/
/*   r0																				*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_SRAM(void); */
.global March_Test_SRAM
.func March_Test_SRAM, March_Test_SRAM
.type March_Test_SRAM, %function
.thumb_func
March_Test_SRAM:

    PUSH  {R1-R7, LR}


    /* Clear test status register */
    movs r7, #0 

		
	/*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_BUFF_ADDR_START
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_BUFF_ADDR_END

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH)

    /* Run Test of reserved RAM area. */
    bl March_Test_Block
.endif  

    /* Check result of reserved RAM testing */ 
    cmp r7, #0
        bne ret_result     
    /*----------------------------------------------------------------------------------*/


    /*-----------------------------  Copy RAM block  -----------------------------------*/	
	
    /* Set parameters to copy tested block of RAM to reserved area */ 
	
	/* Set address of first byte in testing block */  
    mov r0, r11  
    
	/* Set address of last byte in testing block */  
	LDR r1, =TEST_BLOCK_SRAM_SIZE
	ADD r1, r1, r0
    
	/* Set address of buffer first byte */
	LDR r2, =MARCH_BUFF_ADDR_START
	
	/* Check if not end of RAM */
	CMP r2, r1
	BPL Not_end_ram
	
	    /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
	    MOV r1, r2
		
Not_end_ram:
	            
    /* Copy block of SRAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
	/*-----------------------------------------------------------------------------------*/
   
   
    /*-----------------------------  Test block of RAM  ---------------------------------*/
	/* Set address of first byte in testing block */  
	mov r0, r11

/* Disable SRAM March test, if ERROR_IN_STACK_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_STACK_MARCH)

	/* Test Block of RAM */
    bl March_Test_Block
	/*-----------------------------------------------------------------------------------*/  
.endif	
	
	/*-----------------------------  Restore block  -----------------------------------*/
    /* Set address of buffer first byte */
    LDR r2, =MARCH_BUFF_ADDR_START	
   
    /* Copy block of SRAM from Reserved SRAM area */
    bl Copy_Block_From_Buffer      
	/*-----------------------------------------------------------------------------------*/


    /* Check result of RAM block testing */
    cmp r7, #0
        bne ret_result 


    /*-------------------------  Check boundaries  ----------------------------------*/
	/* Save address of first byte for next RAM block */
	mov r11, r1
	
	/* Set address of buffer first byte (this is second last byte for test ) */
    LDR r2, =MARCH_BUFF_ADDR_START	
		
	/* Check if not end of SRAM */
    cmp r2, r1		 	
	    bne Not_end_of_ram


	/*----------------------------  Check results  -------------------------------------*/
        movs r7, #PASS_COMPLETE_STATUS 
        bl ret_result
    
Not_end_of_ram:
    
    movs r7, #PASS_STILL_TESTING_STATUS
    
ret_result:
    
    /* Return result */
	mov r0, r7     
    
    POP   {R1-R7, PC}

.endfunc


/*-------------------------------------------------------------------------------------------*/
/* March_Test_Stack_Init                                                                     */
/*-------------------------------------------------------------------------------------------*/
/*                                                                                           */
/* Summary:                                                                                  */
/*   This function initializes the test STACK base address.                                  */
/*                                                                                           */
/* Parameters:                                                                               */
/*   Function set to variable "Test_Stack_Addr" address of first STACK byte for test.        */         
/*   "Test_Stack_Addr" = MARCH_STACK_BASE + R0. R0 uses to set shift of first byte for test. */
/*                                                                                           */
/* Return:                                                                                   */
/*   void.                                                                                   */
/*                                                                                           */
/* Destroys:                                                                                 */
/*   none.                                                                                   */
/*-------------------------------------------------------------------------------------------*/
/* void March_Test_Stack_Init(uint8); */
.global March_Test_Stack_Init
.func March_Test_Stack_Init, March_Test_Stack_Init
.type March_Test_Stack_Init, %function
.thumb_func
March_Test_Stack_Init:

    PUSH  {R1, R2}

	/* Set base address for March checkerboard test */
	LDR r1, =MARCH_STACK_BASE	
	
	/* Set shift of start test address */ 
	ADD r1, r1, r0
	
 	LDR r2, =Test_Stack_Addr
	STR r1, [r2, #0]
    
	POP  {R1, R2}
	
    BX lr
    
.endfunc
	

/*----------------------------------------------------------------------------------*/
/* March_Test_Stack																	*/
/*----------------------------------------------------------------------------------*/
/* 																					*/
/* Summary:																			*/
/*  This function perform Stack RAM self test using March method. This function 	*/
/*  first duplicates data in the area to be tested as this test is destructive.		*/
/*  At the end of this test the original data is restored to its correct location 	*/
/*  in memory. 																		*/
/*																					*/
/* Parameters:																		*/
/*																					*/
/* Return:																			*/
/*   Result of test:  "1" - fail test;												*/
/*	                  "2" - pass, but still testing status							*/
/*                    "3" - pass and complete status								*/
/* Destroys:																		*/
/*   r0																				*/
/*----------------------------------------------------------------------------------*/
/* uint8 March_Test_Stack(void); */
.global March_Test_Stack
.func March_Test_Stack, March_Test_Stack
.type March_Test_Stack, %function
.thumb_func
March_Test_Stack:

    PUSH  {R1-R7, LR}

    /* Clear test status register */
    movs r7, #0 
	
	
	/*-----------------------------  Test reserved block  ------------------------------*/
    /* Set parameters to test reserved buffer in RAM. */
    /* Write start address of RAM reserved buffer to r0 */
    LDR r0, =MARCH_BUFF_ADDR_START	
 
    /* Write end address of RAM reserved buffer to r1 */
    LDR r1, =MARCH_BUFF_ADDR_STACK_END

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH)

    /* Test reserved RAM area. */
     bl March_Test_Block
.endif    
	
    /* Check result of reserved RAM testing */
     cmp r7, #0
		bne Error_detected_1     
    /*----------------------------------------------------------------------------------*/
	
	
	/*-------------------------  Copy block  ----------------------------------------*/
	
	/* Set parameters to copy tested block of RAM to reserved area */
	
	/* Set address of first byte in testing block */  
    LDR r0, =Test_Stack_Addr
	LDR r0, [r0]
    
    /* Set address of last byte in testing block */  
	LDR r1, =TEST_BLOCK_STACK_SIZE	
	ADD r1, r1, r0
	
	/* Set address of buffer first byte */
	LDR r2, =MARCH_STACK_END	
	
	/* Check if not end of RAM */
	CMP r2, r1	
	BPL Not_end_stack
	
	    /* If 'yes' - Set address of last byte in testing block equal to last byte in RAM */
	    MOV r1, r2
		
Not_end_stack:
	    
	/* Set parameters in r2 to copy tested block of RAM to reserved area */
    LDR r2, =MARCH_BUFF_ADDR_START	
              
	/* Save r0 */
	MOV r5, r0
	
	/* Copy block of stack RAM to Reserved buffer SRAM area */
    bl Copy_Block_To_Buffer        
	/*--------------------------------------------------------------------------------
		
   
    /*-------------------------  Test block  ---------------------------------------*/
    /* Set address of first byte in testing block */
	mov r0, r5

/* Disable STACK March test, if ERROR_IN_SRAM_MARCH enabled */
/* We need it because in both tests used function "March_Test_Block" where error reproduces */
/* If not disconnect this test error reproduced here, but not in the right place */
.if (!ERROR_IN_SRAM_MARCH)

	/* Test block of stack RAM */
    bl March_Test_Block
	/*-------------------------------------------------------------------------------*/
.endif	
	
	/*-------------------------  Restore block  ------------------------------------*/
	/* Write to R0 and R1 parameters for function "Copy_Block_To_Buffer" */
    LDR r2, =MARCH_BUFF_ADDR_START	
    
    /* Copy block of SRAM from Reserved SRAM area */
	bl Copy_Block_From_Buffer      		
	/*-------------------------------------------------------------------------------*/

	/* Return error */
	cmp r7, #0
		bne Error_detected_1     
			
			
	/*-------------------------  Check boundaries  ----------------------------------*/
    /* Set address of  second last byte of STACK */
	LDR r2, =MARCH_STACK_END
	
	/* Check if not end of SRAM	*/
	CMP r2, r1
    BNE Not_end_of_Stack
	    
		
    /*----------------------------  Check results  -----------------------------------*/	
        movs r7, #PASS_COMPLETE_STATUS    
        bl Error_detected_1
		
Not_end_of_Stack:
    
	/* Calculate and save address of first byte for next RAM block */
 	LDR r3, =Test_Stack_Addr
	STR r1, [r3, #0]	
    movs r7, #PASS_STILL_TESTING_STATUS
	
Error_detected_1:

    /* Return result */
	mov r0, r7 
	mov r0, r0
	
    POP   {R1-R7, PC}
    
.endfunc


.end


/* [] END OF FILE */
