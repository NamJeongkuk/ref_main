/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEMAKERMOTORACTIONRESULT_H
#define ICEMAKERMOTORACTIONRESULT_H

#include <stdint.h>

// PSA: This enum impacts the factory test sequence. Add to the end only.

enum
{
   IceMakerMotorActionResult_NoAction = 0,
   IceMakerMotorActionResult_Harvesting,
   IceMakerMotorActionResult_Harvested,
   IceMakerMotorActionResult_BucketWasFull,
   IceMakerMotorActionResult_Homing,
   IceMakerMotorActionResult_Homed,
   IceMakerMotorActionResult_MotorError,
};
typedef uint8_t IceMakerMotorActionResult_t;

#endif
