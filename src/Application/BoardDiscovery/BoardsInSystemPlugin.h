/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef BOARDSINSYSTEMPLUGIN_H
#define BOARDSINSYSTEMPLUGIN_H

#include "BoardsInSystem.h"

typedef struct
{
   struct
   {
      BoardsInSystem_t boardsInSystem;
   } _private;
} BoardsInSystemPlugin_t;

/*!
 * @param instance
 * @param dataModel
 */
void BoardsInSystemPlugin_Init(
   BoardsInSystemPlugin_t *instance,
   I_DataModel_t *dataModel);

#endif
