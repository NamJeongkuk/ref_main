/* File: startup_ARMCM0.S
 * Purpose: startup file for Cortex-M0 devices. Should use with
 *   GCC for ARM Embedded Processors
 * Version: V2.01
 * Date: 12 June 2014
 *
 */
/* Copyright (c) 2011 - 2014 ARM LIMITED
   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/

   .syntax  unified
   .arch    armv6-m

   .text
   .thumb
   .thumb_func
   .align   1
   .globl   Reset_Handler
   .type    Reset_Handler, %function

Reset_Handler:
   .global main

/*  Single section scheme.
 *
 *  The ranges of copy from/to are specified by following symbols
 *    __etext: LMA of start of the section to copy from. Usually end of text
 *    __data_start__: VMA of start of the section to copy to
 *    __data_end__: VMA of end of the section to copy to
 *
 *  All addresses must be aligned to 4 bytes boundary.
 */

   movs  r1, #0
   b     LoopCopyInitializedData

CopyInitializedData:
   ldr   r3, =__etext
   ldr   r3, [r3, r1]
   str   r3, [r0, r1]
   adds  r1, r1, #4

LoopCopyInitializedData:
   ldr   r0, =__data_start__
   ldr   r3, =__data_end__
   adds  r2, r0, r1
   cmp   r2, r3
   bcc   CopyInitializedData
   ldr   r2, =__bss_start__
   b     LoopClearUninitializedData

/*  Single BSS section scheme.
 *
 *  The BSS section is specified by following symbols
 *    __bss_start__: start of the BSS section.
 *    __bss_end__: end of the BSS section.
 *
 *  Both addresses must be aligned to 4 bytes boundary.
 */

ClearUninitializedData:
   movs  r3, #0
   str   r3, [r2]
   adds  r2, r2, #4

LoopClearUninitializedData:
   ldr   r3, = __bss_end__
   cmp   r2, r3
   bcc   ClearUninitializedData

InitializeStackPointer:
   ldr   r0, =__StackBottom
   mov   sp, r0

RelocateVectorTable:
   ldr   r1, =__Vectors_Start
   ldr   r2, =0xE000ED08 // Address of VTOR, Check the ARM Architecture Reference Manual
   str   r1, [r2]

JumpToMain:
   bl    main

   .pool
   .size Reset_Handler, . - Reset_Handler
   .end
