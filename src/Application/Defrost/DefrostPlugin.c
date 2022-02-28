/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "DefrostPlugin.h"
#include "Defrost.h"
#include "DefrostStateOnCompareMatch.h"

static struct
{
   Defrost_t defrost;
} instance;

void DefrostPlugin_Init(I_DataModel_t *dataModel)
{
   DefrostStateOnCompareMatch(dataModel);

   Defrost_Init(&instance.defrost, dataModel);
}
