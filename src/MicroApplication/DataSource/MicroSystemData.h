/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROSYSTEMDATA_H
#define MICROSYSTEMDATA_H

#include "TinyDataSource_Ram.h"
#include "TinyTimer.h"
#include "MicroSystemErds.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

typedef struct
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} TinyRamDataSourceStorage_t;

typedef struct
{
   struct
   {
      TinyDataSource_Ram_t tinyRamDataSource;
      TinyRamDataSourceStorage_t erdRam;
   } _private;
} MicroSystemData_t;

/*!
 * @param instance
 * @param timerModule
 */
void MicroSystemData_Init(MicroSystemData_t *instance, TinyTimerModule_t *timerModule);

/*!
 * @param instance
 * @return
 */
I_TinyDataSource_t *MicroSystemData_DataSource(MicroSystemData_t *instance);

#endif
