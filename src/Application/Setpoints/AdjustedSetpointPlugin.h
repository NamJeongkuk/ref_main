/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADJUSTEDSETPOINTPLUGIN_H
#define ADJUSTEDSETPOINTPLUGIN_H

#include "I_DataModel.h"
#include "AdjustedSetpointData.h"
#include "ErdWriterOnCompareMatch.h"

typedef struct
{
   struct
   {
      I_DataModel_t *dataModel;
      const AdjustedSetpointData_t *adjustedSetpointData;
      ErdWriterOnCompareMatch_t erdWriterOnCompareMatchForAdjustedSetpoint;
   } _private;
} AdjustedSetpointPlugin_t;

/*!
 * Initialize the AdjustedSetpointPlugin
 * @param instance
 * @param dataModel
 */
void AdjustedSetpointPlugin_Init(AdjustedSetpointPlugin_t *instance, I_DataModel_t *dataModel);

#endif
