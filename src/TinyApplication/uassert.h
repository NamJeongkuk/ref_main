/*!
 * @file
 * @brief Uassert
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef UASSERT_H
#define UASSERT_H

#include <stdbool.h>

#define uassert(condition) TinyUassert((bool)(!!(condition)))
extern void TinyUassert(bool condition);

#endif
