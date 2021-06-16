/*!
 * @file
 * @brief File was generated using E2Studio, but has been modified to remove any
 * CPP references and other stuff thats not needed.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

/************************************************************************/
/*    File Version: V1.01                                               */
/*    Date Generated: 04/03/2015                                        */
/************************************************************************/

    /*reset_program.asm*/

    .list
    .section .text
    .global _PowerON_Reset

    .extern _data
    .extern _mdata
    .extern _ebss
    .extern _bss
    .extern _edata
    .extern _main
    .extern _ustack
    .extern _istack
    .extern _rvectors
    .extern _exit

_PowerON_Reset:
/* initialise user stack pointer */
    mvtc    #_ustack,USP

/* initialise interrupt stack pointer */
    mvtc    #_istack,ISP

/* Setup Fixed Vector Table    */
    mvtc    #__fixedVectorTableStart, extb

/* setup relocateable vector table */
    mvtc    #__relocatableVectorTableStart, intb

/* load data section from ROM to RAM */

    mov     #_mdata,r2      /* src ROM address of data section in R2 */
    mov     #_data,r1       /* dest start RAM address of data section in R1 */
    mov     #_edata,r3      /* end RAM address of data section in R3 */
    sub     r1, r3          /* size of data section in R3 (R3=R3-R1) */
    cmp     #0, r3
    beq     2f

1:  mov.b   [r2+], r5
    mov.b   r5, [r1+]
    sub     #1, r3
    bne     1b
2:

/* bss initialisation : zero out bss */
    mov    #00h,r2      /* load R2 reg with zero */
    mov    #_ebss, r3  /* store the end address of bss in R3 */
    mov    #_bss, r1     /* store the start address of bss in R1 */
    sub    r1,r3           /* size of bss section in R3 (R3=R3-R1) */
    sstr.b

    nop
    mvtc #0, psw

/* start user program */
    mov    #_main,r7
    jsr    r7
    mov    #_exit,r7
    jsr    r7

/* call to exit*/
_exit:
    bra  _loop_here
_loop_here:
    bra _loop_here

    .text
    .end
