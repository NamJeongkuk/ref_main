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

typedef struct
{
   struct
   {
      DispenseSelectionRequestHandler_t dispenseSelection;
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
