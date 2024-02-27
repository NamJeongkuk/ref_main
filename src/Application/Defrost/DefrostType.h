/*!
 * @file
 * @brief Defrost type definition
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTTYPE_H
#define DEFROSTTYPE_H

#include <stdint.h>

enum
{
   DefrostType_SecondaryOnly,
   DefrostType_Full,
   DefrostType_DontCare
};
typedef uint8_t DefrostType_t;

#endif
