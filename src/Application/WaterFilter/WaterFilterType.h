/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WATERFILTERTYPE_H
#define WATERFILTERTYPE_H

#include <stdint.h>

enum
{
   WaterFilterType_RPWF = 0,
   WaterFilterType_RPWFE,
   WaterFilterType_MWF,
   WaterFilterType_MWFE,
   WaterFilterType_XWF,
   WaterFilterType_XWFE
};
typedef uint8_t WaterFilterType_t;

#endif
