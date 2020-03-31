/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Dtc.h"
#include "iodefine.h"

extern void *const dtcVectorTable[];

void Dtc_Init(void)
{
   DTC.DTCST.BYTE = 0x00; // DTC module stop
   DTC.DTCCR.BIT.RRS = 0; // Transfer data read is not skipped
   DTC.DTCADMOD.BYTE = 0x00; // Full-address mode

   DTC.DTCVBR = &dtcVectorTable; // Set the vector table base address
   DTC.DTCCR.BIT.RRS = 1; // Transfer data read is skipped when vector numbers match

   DTC.DTCST.BYTE = 0x01; // DTC module start
}
