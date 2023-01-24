/*!
 * @file
 * @brief Rake position type
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RAKEPOSITION_H
#define RAKEPOSITION_H

#include <stdint.h>

enum
{
   RakePosition_NotHome,
   RakePosition_Home
};
typedef uint8_t RakePosition_t;

#endif