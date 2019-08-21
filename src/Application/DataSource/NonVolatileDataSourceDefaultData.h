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
 * Set data to zeros
 * @param data Buffer into which to copy the data
 * @param dataSize Size of the buffer
 */
void NonVolatileDataSourceDefaultData_Zeros(void *data, size_t dataSize);

/*!
 * Set data to boolean true
 * @param data Buffer into which to copy the data
 * @param dataSize Size of the buffer
 */
void NonVolatileDataSourceDefaultData_BooleanTrue(void *data, size_t dataSize);

#endif
