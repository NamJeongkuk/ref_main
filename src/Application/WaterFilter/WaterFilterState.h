/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERSTATE_H
#define WATERFILTERSTATE_H

#include <stdint.h>

enum
{
   WaterFilterState_Good = 0,
   WaterFilterState_Replace,
   WaterFilterState_Expired,
   WaterFilterState_Bypass,
   WaterFilterState_Leak,
   WaterFilterState_Error
};
typedef uint8_t WaterFilterState_t;

#endif
