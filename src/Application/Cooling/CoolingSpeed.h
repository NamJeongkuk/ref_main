/*!
 * @file
 * @brief Type that is used to represent the speed with which the Grid wants to cool
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef COOLINGSPEED_H
#define COOLINGSPEED_H

#include <stdint.h>

enum
{
   CoolingSpeed_Off,
   CoolingSpeed_Low,
   CoolingSpeed_Mid,
   CoolingSpeed_High,
   CoolingSpeed_PullDown
};
typedef uint8_t CoolingSpeed_t;

#endif
