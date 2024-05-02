/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODEPLUGIN_H
#define SERVICEMODEPLUGIN_H

#include "I_DataModel.h"
#include "I_ServiceTest.h"
#include "ServiceModeRequestHandler.h"
#include "ServiceModeTest_BoardVersion.h"
#include "ServiceModeTest_ExitServiceMode.h"
#include "ServiceModeTest_Thermistor.h"
#include "ServiceModeTest_Version.h"

typedef struct
{
   struct
   {
      ServiceModeRequestHandler_t serviceModeRequestHandler;

      struct
      {
         ServiceModeTest_ExitServiceMode_t test;
         ServiceModeTestComponent_t component;
      } exitServiceMode;

      struct
      {
         ServiceModeTest_Version_t test;
         ServiceModeTestComponent_t component;
      } mainBoardVersion;

      struct
      {
         ServiceModeTest_BoardVersion_t test;
         ServiceModeTestComponent_t component;
      } externalBoardsVersion;

      struct
      {
         ServiceModeTest_Thermistor_t test;
         ServiceModeTestComponent_t component;
      } thermistorsTest;
   } _private;
} ServiceModePlugin_t;

void ServiceModePlugin_Init(ServiceModePlugin_t *instance, I_DataModel_t *dataModel);

#endif
