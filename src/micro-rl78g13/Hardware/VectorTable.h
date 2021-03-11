/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef VECTORTABLE_H
#define VECTORTABLE_H

#include <stdint.h>

/*!
 * @param vectorTable
 */
static inline void VectorTable_SetVectorTable(const void *const *vectorTable)
{
   extern uint8_t _br;
   extern const void *const *_vtor;

   _br = 0xED; // Opcode for unconditional jump to a 16-bit address
   _vtor = vectorTable;
}

#endif
