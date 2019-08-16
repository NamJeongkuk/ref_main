/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FAULTIDS_H
#define FAULTIDS_H

#include <stdint.h>

enum
{
   FaultId_Unknown = 0,
   FaultId_SomeFault = 1,
   FaultId_SomeOtherFault = 1,
};
typedef uint16_t FaultId_t;

#endif
