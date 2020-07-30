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
#include "I_Output.h"

#define uassert(condition) \
   CONCAT(addr, __LINE__) : __uassert_func(__FILE__, __LINE__, (bool)(!!(condition)), #condition, &&CONCAT(addr, __LINE__));

extern void __uassert_func(const char *fileName, int lineNumber, bool condition, const char *conditionString, const void *programCounter);

void Uassert_Init(I_Output_t *_programCounterAddressOutput);

#endif
