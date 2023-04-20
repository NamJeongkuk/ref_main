/*!
 * @file
 * @brief This plugin contains the SideBySideDispensePlugin type which stores the DispenseSelectionRequestHandler type.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SIDEBYSIDEDISPENSEPLUGIN_H
#define SIDEBYSIDEDISPENSEPLUGIN_H

#include "I_DataModel.h"
#include "DispenseSelectionRequestHandler.h"
#include "DispensingRequestHandler.h"
#include "DispenseController.h"
#include "DoorInhibitDispensingResolver.h"

typedef struct
{
   struct
   {
      DispenseSelectionRequestHandler_t dispenseSelectionRequestHandler;
      DispensingRequestHandler_t dispensingRequestHandler;
      DispenseController_t dispenseController;
      DoorInhibitDispensingResolver_t doorInhibitWaterDispensingResolver;
      DoorInhibitDispensingResolver_t doorInhibitIceDispensingResolver;
   } _private;
} SideBySideDispensePlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void SideBySideDispensePlugin_Init(
   SideBySideDispensePlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
