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
   ENTRY(Erd_BuildNumber,                 0x0031, uint32_t ) \
   ENTRY(Erd_ApplicationVersion,          0x003A, Version_t) \

#define EXPAND_AS_ENUM(Name, Number, DataType) \
   Name = Number COMMA

enum
{
   ERD_TABLE(EXPAND_AS_ENUM)
};

#endif
