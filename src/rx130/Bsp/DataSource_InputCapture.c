/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DataSource_InputCapture.h"
#include "utils.h"
#include "uassert.h"
#include "InputCaptureType.h"
#include "I_InputCapture.h"
#include "SystemErds.h"

#define ERD_IS_IN_RANGE(erd) (BETWEEN(Erd_BspInputCapture_Start, erd, Erd_BspInputCapture_End))

static struct
{
   I_DataSource_t interface;
   Event_Synchronous_t *onChangeEvent;
} instance;

static void Read(I_DataSource_t *_instance, const Erd_t erd, void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static void Write(I_DataSource_t *_instance, const Erd_t erd, const void *data)
{
   IGNORE(_instance);
   IGNORE(erd);
   IGNORE(data);
}

static bool Has(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   return ERD_IS_IN_RANGE(erd);
}

static uint8_t SizeOf(I_DataSource_t *_instance, const Erd_t erd)
{
   IGNORE(_instance);
   IGNORE(erd);
   return sizeof(InputCaptureMode_t);
}

static const I_DataSource_Api_t api = { Read, Write, Has, SizeOf };

I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
   IGNORE(timerModule);
   instance.interface.api = &api;
   instance.interface.OnDataChange = &onChangeEvent->interface;
   instance.onChangeEvent = onChangeEvent;

   return &instance.interface;
}
