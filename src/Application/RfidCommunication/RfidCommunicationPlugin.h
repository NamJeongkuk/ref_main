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

typedef struct
{
   struct
   {
      RfidCommunicator_t rfidCommunicator;
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