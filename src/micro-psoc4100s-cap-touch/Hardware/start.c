/***************************************************************************/ /**
 * \file Cm0Start.c
 * \version 5.70
 *
 * \brief Startup code for the ARM CM0.
 *
 ********************************************************************************
 * \copyright
 * Copyright 2010-2018, Cypress Semiconductor Corporation.  All rights reserved.
 * You may use this file only in accordance with the license, terms, conditions,
 * disclaimers, and limitations in the end user license agreement accompanying
 * the software package with which this file was provided.
 *******************************************************************************/

#include <limits.h>
#include "cydevice_trm.h"
#include "cytypes.h"
#include "cyfitter_cfg.h"
#include "CyLib.h"
#include "cyfitter.h"

// fixme use linker symbol
#define VECTOR_TABLE_ADDRESS (0x00000300)

#define CY_NUM_VECTORS (CY_INT_IRQ_BASE + CY_NUM_INTERRUPTS)
#define CY_NUM_ROM_VECTORS (4u)

#define CY_SYS_INITIAL_STACK_POINTER (&__cy_stack)

#include <errno.h>
extern int end;

void initialize_psoc(void);
void PowerOnReset(void);
void NmiHandler(void);
void HardfaultHandler(void);

void Gea3_SPI_UART_ISR(void);
void SysTickHandler(void);
void CapSenseIsrTrampoline(void);

void initialize_psoc(void);

CY_NORETURN
CY_ISR(IntDefaultHandler)
{
   while(1)
      ;
}

/* Stack Base address */
extern void __cy_stack(void);

/* Application entry point. */
extern int main(void);

/* The static objects constructors initializer */
extern void __libc_init_array(void);

typedef unsigned char __cy_byte_align8 __attribute((aligned(8)));

struct __cy_region
{
   __cy_byte_align8 *init; /* Initial contents of this region.  */
   __cy_byte_align8 *data; /* Start address of region.  */
   size_t init_size; /* Size of initial data.  */
   size_t zero_size; /* Additional size to be zeroed.  */
};

extern const struct __cy_region __cy_regions[];
extern const char __cy_region_num __attribute__((weak));
#define __cy_region_num ((size_t)&__cy_region_num)

/*******************************************************************************
 * Function Name: Start_c
 ****************************************************************************/
/**
 *
 * This function handles initializing the .data and .bss sections in
 * preparation for running the standard c code.  Once initialization is complete
 * it will call main().  This function will never return.
 *
 *******************************************************************************/
void Start_c(void) __attribute__((noreturn, noinline));
void Start_c(void)
{
   unsigned regions = __cy_region_num;
   const struct __cy_region *rptr = __cy_regions;

   /* Initialize memory */
   for(regions = __cy_region_num; regions != 0u; regions--)
   {
      uint32 *src = (uint32 *)rptr->init;
      uint32 *dst = (uint32 *)rptr->data;
      unsigned limit = rptr->init_size;
      unsigned count;

      for(count = 0u; count != limit; count += sizeof(uint32))
      {
         *dst = *src;
         dst++;
         src++;
      }
      limit = rptr->zero_size;
      for(count = 0u; count != limit; count += sizeof(uint32))
      {
         *dst = 0u;
         dst++;
      }

      rptr++;
   }

   /* Invoke static objects constructors */
   cyfitter_cfg();

   (*(uint32 *)CYREG_CM0P_VTOR) = VECTOR_TABLE_ADDRESS;
   (void)main();

   while(1)
      ;
}

/*******************************************************************************
 * Rom Interrupt Vector table storage area. Must be 256-byte aligned.
 *******************************************************************************/
__attribute__((used, section(".fixedVectorTable")))
const cyisraddress TempRomVectors[CY_NUM_VECTORS] =
   {
      CY_SYS_INITIAL_STACK_POINTER,
      (cyisraddress)&PowerOnReset
   };

__attribute__((used, section(".vectors")))
const cyisraddress RomVectors[CY_NUM_VECTORS] =
   {
      CY_SYS_INITIAL_STACK_POINTER,
      (cyisraddress)&PowerOnReset,
      (cyisraddress)&NmiHandler,
      (cyisraddress)&HardfaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      (cyisraddress)&SysTickHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      (cyisraddress)&Gea3_SPI_UART_ISR,
      &IntDefaultHandler,
      (cyisraddress)&CapSenseIsrTrampoline,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
      &IntDefaultHandler,
   };

/*******************************************************************************
 * Function Name: initialize_psoc
 ****************************************************************************/
/**
 *
 * This function is used to initialize the PSoC chip before calling main.
 *
 *******************************************************************************/
__attribute__((constructor(101))) void initialize_psoc(void)
{
   /* Initialize configuration registers. */
   cyfitter_cfg();
   (*(uint32 *)CYREG_CM0P_VTOR) = VECTOR_TABLE_ADDRESS;
}

void PowerOnReset(void)
{
   CySysWdtDisable();
   Start_c();
}

void NmiHandler(void)
{
   while(1)
      ;
}

void HardfaultHandler(void)
{
   while(1)
      ;
}
