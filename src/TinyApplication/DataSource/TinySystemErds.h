/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYSYSTEMERDS_H
#define TINYSYSTEMERDS_H

#include "Version.h"
#include "XMacroUtils.h"
#include <stdint.h>

// Name, Number, DataType
#define ERD_TABLE(ENTRY) \
   ENTRY(Erd_ApplicationVersion,       0x003A, Version_t) \
   ENTRY(Erd_ButtonState,              0xF000, bool) \
   ENTRY(Erd_LedState,                 0xF001, bool) \

#define EXPAND_AS_ENUM(Name, Number, DataType) \
   Name = Number COMMA

enum
{
   ERD_TABLE(EXPAND_AS_ENUM)
};

#endif
