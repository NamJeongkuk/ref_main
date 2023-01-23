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

#ifndef OLD_HW

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_NAMES(name, port, bit, timer, channel, peripheralFunction) \
   name,

#define INPUTCAPTURE_TABLE_EXPAND_AS_ERD_MODE(name, port, bit, timer, channel, peripheralFunction) \
   name##_Mode,

// clang-format off
enum
{
   Erd_BspInputCapture_Start = (BspErdInputCaptureStart - 1),
   Erd_BspInputCapture_InputErd_Start = Erd_BspInputCapture_Start,
   INPUT_CAPTURE_TABLE(INPUTCAPTURE_TABLE_EXPAND_AS_ERD_NAMES)
   Erd_BspInputCapture_InputErd_End
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

#endif

I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent);

#endif
