/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROST_H
#define DEFROST_H

#include "I_DataModel.h"
#include "DefrostData.h"

typedef struct
{
   struct
   {
      const DefrostData_t *defrostParametricData;
   } _private;
} Defrost_t;

void Defrost_Init(Defrost_t *instance, I_DataModel_t *dataModel);

#endif
