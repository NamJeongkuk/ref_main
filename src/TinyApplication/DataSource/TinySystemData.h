/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TINYSYSTEMDATA_H
#define TINYSYSTEMDATA_H

#include "TinyDataSource_Ram.h"
#include "TinySystemErds.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType) \
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
} TinySystemData_t;

/*!
 * @param instance
 */
void TinySystemData_Init(TinySystemData_t *instance);

/*!
 * @param instance
 * @return
 */
I_TinyDataSource_t *TinySystemData_DataSource(TinySystemData_t *instance);

#endif
