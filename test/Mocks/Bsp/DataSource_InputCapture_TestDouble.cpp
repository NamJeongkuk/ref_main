/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "SystemErds.h"
#include "DataSource_InputCapture.h"
}

#include "DataSource_NonMockTestDouble.h"

#define EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS(inputName, port, bit, timer, channel, peripheralFunction) \
   {                                                                                                          \
      .erd = inputName, .sizeOfEntity = sizeof(InputCaptureMicroSeconds_t)                                    \
   },

static struct
{
   DataSource_NonMockTestDouble_t dataSourceNonMockTestDouble;
} instance;

static const DataSource_NonMockTestDoubleItem_t inputCaptureItems[] = {
   INPUT_CAPTURE_TABLE(EXPAND_AS_DATASOURCE_NONMOCKTESTDOUBLEITEMS)
};

extern "C" I_DataSource_t *DataSource_InputCapture_Init(
   TimerModule_t *timerModule,
   Event_Synchronous_t *onChangeEvent)
{
   (void)timerModule;
   (void)onChangeEvent;

   DataSource_NonMockTestDouble_Init(
      &instance.dataSourceNonMockTestDouble,
      inputCaptureItems,
      NUM_ELEMENTS(inputCaptureItems));

   return &instance.dataSourceNonMockTestDouble.interface;
}
