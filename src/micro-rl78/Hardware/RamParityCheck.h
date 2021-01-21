/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAMPARITYCHECK_H
#define RAMPARITYCHECK_H

#include "iodefine_ext.h"

/*!
 */
static inline void RamParityCheck_Disable(void)
{
   RPERDIS = 1;
}

#endif
