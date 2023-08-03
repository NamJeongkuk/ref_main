/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SNACKMODEENABLEREQUEST_H
#define SNACKMODEENABLEREQUEST_H

#include <stdint.h>

enum
{
   SnackModeEnableRequest_Off = 0,
   SnackModeEnableRequest_On = 1,
   SnackModeEnableRequest_Ready = 0xFF
};
typedef uint8_t SnackModeEnableRequest_t;

#endif
