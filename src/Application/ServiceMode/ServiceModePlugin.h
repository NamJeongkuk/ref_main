/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODEPLUGIN_H
#define SERVICEMODEPLUGIN_H

#include "I_DataModel.h"
#include "ServiceModeRequestHandler.h"
#include "ServiceModeTest_BoardVersion.h"
#include "ServiceModeTest_ExitServiceMode.h"
#include "ServiceModeTest_Version.h"
#include "I_ServiceTest.h"

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
      } mainBoardApplicationVersion;

      struct
      {
         ServiceModeTest_Version_t test;
         ServiceModeTestComponent_t component;
      } mainBoardParametricVersion;

      struct
      {
         ServiceModeTest_BoardVersion_t test;
         ServiceModeTestComponent_t component;
      } dispenserBoardVersion;

      struct
      {
         ServiceModeTest_BoardVersion_t test;
         ServiceModeTestComponent_t component;
      } internalTempBoardUiVersion;

      struct
      {
         ServiceModeTest_BoardVersion_t test;
         ServiceModeTestComponent_t component;
      } wifiBoardVersion;

      struct
      {
         ServiceModeTest_BoardVersion_t test;
         ServiceModeTestComponent_t component;
      } rfidBoardVersion;
   } _private;
} ServiceModePlugin_t;

void ServiceModePlugin_Init(ServiceModePlugin_t *instance, I_DataModel_t *dataModel);

#endif
