/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERLIFESTATUS_H
#define WATERFILTERLIFESTATUS_H

#include <stdint.h>

enum
{
   WaterFilterLifeStatus_NoOrderNecessary,
   WaterFilterLifeStatus_Replace,
   WaterFilterLifeStatus_Expired
};
typedef uint8_t WaterFilterLifeStatus_t;

#endif
