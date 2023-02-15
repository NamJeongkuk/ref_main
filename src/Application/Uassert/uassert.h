/*!
 * @file
 * @brief User-defined assert mechanism
 *
 * Copyright GE Appliances - Confidential - All rights reserved
 */

#ifndef UASSERT_H
#define UASSERT_H

#include <stdbool.h>
#include "utils.h"
#include "ProgramCounterAddress.h"

#define uassert(condition) \
   CONCAT(addr, __LINE__) : __uassert_func((bool)(!!(condition)), (ProgramCounterAddress_t)(uintptr_t)(&&CONCAT(addr, __LINE__)));

extern void __uassert_func(bool condition, ProgramCounterAddress_t programCounter);

#endif
