/*!
 * @file
 * @brief Provides the MC heartbeat and ERD stream.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MCCOMMUNICATIONPLUGIN_H
#define MCCOMMUNICATIONPLUGIN_H

#include "I_TinyDataSource.h"
#include "I_TinyGea2Interface.h"

/*!
 * @param dataSource
 * @param externalDataSource
 * @param gea2Interface
 */
void McCommunicationPlugin_Init(
   I_TinyDataSource_t *dataSource,
   I_TinyDataSource_t *externalDataSource,
   I_TinyGea2Interface_t *gea2Interface);

#endif
