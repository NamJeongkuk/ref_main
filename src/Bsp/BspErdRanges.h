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
   BspErdAdcStart = BspErdGpioEnd + 1,
   BspErdAdcEnd = BspErdAdcStart + BspErdRangeSize,
   BspErdPwmStart = BspErdAdcEnd + 1,
   BspErdPwmEnd = BspErdPwmStart + BspErdRangeSize,
   BspErdInputCaptureStart = BspErdPwmEnd + 1,
   BspErdInputCaptureEnd = BspErdInputCaptureStart + BspErdRangeSize,
   BspErdPersonalityStart = BspErdInputCaptureEnd + 1,
   BspErdPersonalityEnd = BspErdPersonalityStart + BspErdRangeSize
};

#endif
