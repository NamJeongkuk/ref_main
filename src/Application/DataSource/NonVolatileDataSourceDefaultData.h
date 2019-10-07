/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef NONVOLATILEDATASOURCEDEFAULTDATA_H
#define NONVOLATILEDATASOURCEDEFAULTDATA_H

#include <stddef.h>

/*!
 * @param data
 * @param dataSize
 */
void NonVolatileDataSourceDefaultData_Zeros(void *data, size_t dataSize);

/*!
 * @param data
 * @param dataSize
 */
void NonVolatileDataSourceDefaultData_BooleanTrue(void *data, size_t dataSize);

/*!
 * @param data
 * @param dataSize
 */
void NonVolatileDataSourceDefaultData_Code(void *data, size_t dataSize);

#endif
