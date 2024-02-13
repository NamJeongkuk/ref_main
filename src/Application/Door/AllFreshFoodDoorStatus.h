/*!
 * @file
 * @brief Object used to update if any Fresh Food door is opened
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ALLFRESHFOODDOORSTATUS_H
#define ALLFRESHFOODDOORSTATUS_H

#include "I_DataModel.h"

typedef struct
{
   Erd_t rightFreshDoorStatusErd; // bool
   Erd_t leftFreshDoorStatusErd; // bool
   Erd_t allFreshFoodDoorsAreClosedErd; // bool
} AllFreshFoodDoorStatusConfiguration_t;

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      EventSubscription_t dataModelChangedSubscription;
      const AllFreshFoodDoorStatusConfiguration_t *config;
   } _private;
} AllFreshFoodDoorStatus_t;

/*!
 * Initializes a fresh food door status update
 * @param instance The object instance
 * @param dataModel The data model
 * @param config Configuration of erds for module
 */
void AllFreshFoodDoorStatus_Init(
   AllFreshFoodDoorStatus_t *instance,
   I_DataModel_t *dataModel,
   const AllFreshFoodDoorStatusConfiguration_t *config);

#endif
