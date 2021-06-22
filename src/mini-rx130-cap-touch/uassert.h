/*!
 * @file
 * @brief User-defined assert mechanism
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef UASSERT_H_
#define UASSERT_H_

#include <stdbool.h>

#define uassert(condition) __uassert_func((bool)(condition))

extern void __uassert_func(bool condition);

#endif
