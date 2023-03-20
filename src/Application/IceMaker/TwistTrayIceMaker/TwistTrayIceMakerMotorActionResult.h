/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYMOTORACTIONRESULT_H
#define TWISTTRAYMOTORACTIONRESULT_H

#include <stdint.h>

enum
{
   TwistTrayIceMakerMotorActionResult_NoAction = 0,
   TwistTrayIceMakerMotorActionResult_Harvesting,
   TwistTrayIceMakerMotorActionResult_Harvested,
   TwistTrayIceMakerMotorActionResult_BucketWasFull,
   TwistTrayIceMakerMotorActionResult_Homing,
   TwistTrayIceMakerMotorActionResult_Homed,
   TwistTrayIceMakerMotorActionResult_MotorError,
};
typedef uint8_t TwistTrayIceMakerMotorActionResult_t;

#endif
