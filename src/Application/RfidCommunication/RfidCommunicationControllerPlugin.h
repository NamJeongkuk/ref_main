/*!
 * @file
 * @brief Plugin for RFID Communication Controller State Machine
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDCOMMUNICATIONCONTROLLERPLUGIN_H
#define RFIDCOMMUNICATIONCONTROLLERPLUGIN_H

#include "I_DataModel.h"
#include "RfidCommunicationController.h"

typedef struct
{
   struct
   {
      RfidCommunicationController_t rfidCommunicationController;
   } _private;
} RfidCommunicationControllerPlugin_t;

/*!
 * @brief
 * @param instance
 * @param dataModel
 */
void RfidCommunicationControllerPlugin_Init(
   RfidCommunicationControllerPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
