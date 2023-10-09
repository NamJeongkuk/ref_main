/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDCOMMUNICATIONPLUGIN_H
#define RFIDCOMMUNICATIONPLUGIN_H

#include "I_DataModel.h"
#include "RfidCommunicator.h"
#include "RfidCommunicationControllerPlugin.h"

typedef struct
{
   struct
   {
      RfidCommunicator_t rfidCommunicator;
      RfidCommunicationControllerPlugin_t rfidCommunicationControllerPlugin;
      EventSubscription_t rfidBoardInSystemSubscription;
      I_DataModel_t *dataModel;
      I_DataSource_t *externalDataSource;
   } _private;
} RfidCommunicationPlugin_t;

/*!
 * @brief
 * @param instance
 * @param dataModel
 */
void RfidCommunicationPlugin_Init(
   RfidCommunicationPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
