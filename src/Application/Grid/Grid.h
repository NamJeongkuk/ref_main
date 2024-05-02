/*!
 * @file
 * @brief Main Grid Functionality
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef GRID_H
#define GRID_H

#include <stdint.h>
#include "I_DataModel.h"
#include "Timer.h"
#include "DeltaGridLines.h"
#include "PersonalityParametricData.h"

typedef struct
{
   Erd_t timerModuleErd;
   Erd_t gridOverrideSignalErd; // Signal_t
   Erd_t gridOverrideEnableErd; // bool
} GridConfiguration_t;

typedef struct
{
   struct
   {
      const GridConfiguration_t *configuration;
      const GridData_t *gridData;
      I_DataModel_t *dataModel;
      Timer_t gridTimer;
      EventSubscription_t gridOverrideEnableSubscription;
      EventSubscription_t gridOverrideSignalSubscription;

      /*!
       * Grid function to run.
       * @pre Run(context)
       * @param context DataModel context object
       */
      void (*gridFunction)(void *context);
   } _private;
} Grid_t;

/*!
 * Initialize a Grid Module
 * @param instance The object instance.
 * @param dataModel instance of dataModel
 * @param configuration contains erds and other unique configs for instance.
 * @param gridData contains grid data information for specific grid in use.
 * @param gridFunction "run" function of the grid to use
 */
void Grid_Init(
   Grid_t *instance,
   I_DataModel_t *dataModel,
   const GridConfiguration_t *configuration,
   const GridData_t *gridData,
   void (*gridFunction)(void *context));

#endif
