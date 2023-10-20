/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTEDDATASOURCE_H
#define CONVERTEDDATASOURCE_H

#include "DataSource_Converted.h"

typedef struct
{
   struct
   {
      DataSource_Converted_t dataSource;
   } _private;
} ConvertedDataSource_t;

/*!
 * @param instance
 * @param toConvert
 */
void ConvertedDataSource_Init(ConvertedDataSource_t *instance, I_DataSource_t *toConvert);

/*!
 * @param instance
 * @return
 */
I_DataSource_t *ConvertedDataSource_DataSource(ConvertedDataSource_t *instance);

#endif
