/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef SERVICEMODEREQUESTHANDLER_H
#define SERVICEMODEREQUESTHANDLER_H

#include "I_DataModel.h"
#include "I_ServiceTest.h"
#include "EventSubscription.h"
#include "ServiceModeTestNumber.h"
#include "LinkedList.h"

typedef struct
{
   LinkedListNode_t node;
   I_ServiceTest_t *test;
} ServiceModeTestComponent_t;

typedef struct
{
   struct
   {
      EventSubscription_t serviceTestRequestChanged;
      ServiceTestResources_t resources;
      LinkedList_t components;
   } _private;
} ServiceModeRequestHandler_t;

/*!
 * @param instance
 * @param dataModel
 * @param maxTestNumber
 */
void ServiceModeRequestHandler_Init(
   ServiceModeRequestHandler_t *instance,
   I_DataModel_t *dataModel,
   ServiceModeTestNumber_t maxTestNumber);

/*!
 * @param instance
 * @param test
 * @param component
 */
void ServiceModeRequestHandler_AddTest(
   ServiceModeRequestHandler_t *instance,
   I_ServiceTest_t *test,
   ServiceModeTestComponent_t *component);

#endif
