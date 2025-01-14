/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BSPERDRANGES_H
#define BSPERDRANGES_H

enum
{
   BspErdStart = 0xF600,
   BspErdRangeSize = 0x100,
};

enum
{
   BspErdGpioStart = BspErdStart,
   BspErdGpioEnd = BspErdGpioStart + BspErdRangeSize,
   BspErdSoftPwmGpioStart = BspErdGpioEnd + 1,
   BspErdSoftPwmGpioEnd = BspErdSoftPwmGpioStart + BspErdRangeSize,
   BspErdAdcStart = BspErdSoftPwmGpioEnd + 1,
   BspErdAdcEnd = BspErdAdcStart + BspErdRangeSize,
   BspErdPwmStart = BspErdAdcEnd + 1,
   BspErdPwmEnd = BspErdPwmStart + BspErdRangeSize,
   BspErdRampingLedPwmStart = BspErdPwmEnd + 1,
   BspErdRampingLedPwmEnd = BspErdRampingLedPwmStart + BspErdRangeSize,
   BspErdInputCaptureStart = BspErdRampingLedPwmEnd + 1,
   BspErdInputCaptureEnd = BspErdInputCaptureStart + BspErdRangeSize,
   BspErdPersonalityStart = BspErdInputCaptureEnd + 1,
   BspErdPersonalityEnd = BspErdPersonalityStart + BspErdRangeSize,
   BspErdTwistTrayMotorStart = BspErdPersonalityEnd + 1,
   BspErdTwistTrayMotorEnd = BspErdTwistTrayMotorStart + BspErdRangeSize
};

#endif
