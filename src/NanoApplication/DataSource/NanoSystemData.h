/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NANOSYSTEMDATA_H
#define NANOSYSTEMDATA_H

#include "TinyDataSource_Ram.h"
#include "NanoSystemErds.h"
#include "utils.h"

#define EXPAND_AS_OFFSET_STRUCT_MEMBER(Name, Number, DataType, Stream, RemoteErd) \
   uint8_t MACRO_SAFE_CONCATENATE(erd, Name)[sizeof(DataType)];

#ifdef LITTLE_ENDIAN
typedef union
{
   ERD_TABLE(EXPAND_AS_OFFSET_STRUCT_MEMBER)
} EndiannessSwapBuffer_t;
#endif

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
#ifdef LITTLE_ENDIAN
      TinyDataSource_EndiannessSwapped_t swappedDataSource;
      EndiannessSwapBuffer_t endiannessSwapBuffer;
#endif
   } _private;
} NanoSystemData_t;

/*!
 * @param instance
 */
void NanoSystemData_Init(NanoSystemData_t *instance);

/*!
 * @param instance
 * @return
 */
I_TinyDataSource_t *NanoSystemData_DataSource(NanoSystemData_t *instance);

/*!
 * @param instance
 * @return
 */
I_TinyDataSource_t *NanoSystemData_ExternalDataSource(NanoSystemData_t *instance);

#endif
