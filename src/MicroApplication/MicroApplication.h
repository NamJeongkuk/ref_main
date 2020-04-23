/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MICROAPPLICATION_H
#define MICROAPPLICATION_H

#include "I_TinyDataSource.h"
#include "I_TinyGea2Interface.h"

/*!
 * @param dataSource
 * @param gea2Interface
 */
void MicroApplication_Init(I_TinyDataSource_t *dataSource, I_TinyGea2Interface_t *gea2Interface);

#endif
