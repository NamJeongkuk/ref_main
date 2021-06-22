/*!
 * @file
 * @brief Provides the MC heartbeat and ERD stream.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MCCOMMUNICATIONPLUGIN_H
#define MCCOMMUNICATIONPLUGIN_H

#include "I_DataSource.h"
#include "I_Gea2PacketEndpoint.h"

/*!
 * @param dataSource
 * @param externalDataSource
 * @param gea2Interface
 */
void McCommunicationPlugin_Init(
   I_DataSource_t *dataSource,
   I_DataSource_t *externalDataSource,
   I_Gea2PacketEndpoint_t *gea2PacketEndpoint);

#endif
