/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DATASOURCE_INPUTCAPTURE
#define DATASOURCE_INPUTCAPTURE

#include "I_DataSource.h"
#include "BspErdRanges.h"
#include "InputCaptureConfiguration.h"
#include "Event_Synchronous.h"
#include "Timer.h"
#include "XMacroUtils.h"
#include "utils.h"

#define IncludeTypeMotor_0(_x) _x
#define IncludeTypeMotor_1(_x)

#define IncludeTypeFan_0(_x)
#define IncludeTypeFan_1(_x) _x

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_NAMES(name, port, bit, timer, channel, type) \
   name,

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_MODE(name, port, bit, timer, channel, type) \
   name##_Mode,

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_TYPE_MOTOR(name, port, bit, timer, channel, type) \
   CONCAT(IncludeTypeMotor_, type)                                                         \
   (name COMMA)

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_TYPE_FAN(name, port, bit, timer, channel, type) \
   CONCAT(IncludeTypeFan_, type)                                                         \
   (name COMMA)

// clang-format off
enum
{
   Erd_BspInputCapture_Start = (BspErdInputCaptureStart - 1),
   Erd_BspInputCapture_InputErd_Start = Erd_BspInputCapture_Start,
   Erd_BspInputCapture_InputErd_Motor_Start = Erd_BspInputCapture_InputErd_Start,
   INPUT_CAPTURE_TABLE(INPUTCAPTURE_TABLE_EXPAND_AS_ERD_TYPE_MOTOR)
   Erd_BspInputCapture_InputErd_Motor_End,
   Erd_BspInputCapture_InputErd_Fan_Start = Erd_BspInputCapture_InputErd_Motor_End - 1,
   INPUT_CAPTURE_TABLE(INPUTCAPTURE_TABLE_EXPAND_AS_ERD_TYPE_FAN)
   Erd_BspInputCapture_InputErd_Fan_End,
   Erd_BspInputCapture_InputErd_End = Erd_BspInputCapture_InputErd_Fan_End
};
// clang-format on

// clang-format off
enum
{
   Erd_BspInputCapture_OutputErd_Start = (Erd_BspInputCapture_InputErd_End - 1),
   INPUT_CAPTURE_TABLE(INPUTCAPTURE_TABLE_EXPAND_AS_ERD_MODE)
   Erd_BspInputCapture_OutputErd_End,
   Erd_BspInputCapture_End = Erd_BspInputCapture_OutputErd_End
};
// clang-format on

I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent);

#endif
