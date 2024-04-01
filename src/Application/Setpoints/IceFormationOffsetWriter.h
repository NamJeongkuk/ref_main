/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ICEFORMATIONOFFSETWRITER_H
#define ICEFORMATIONOFFSETWRITER_H

#include "I_DataModel.h"

typedef struct
{
   struct
   {
      EventSubscription_t subscription;
   } _private;
} IceFormationOffsetWriter_t;

/*!
 * @param instance
 * @param dataModel
 */
void IceFormationOffsetWriter_Init(
   IceFormationOffsetWriter_t *instance,
   I_DataModel_t *dataModel);

#endif
