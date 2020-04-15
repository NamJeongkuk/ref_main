/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MicroApplication.h"
#include "MicroSystemErds.h"
#include "utils.h"

void MicroApplication_Init(MicroApplication_t *instance, I_TinyDataSource_t *dataSource)
{
   IGNORE(instance);

   ApplianceApiPlugin_Init(dataSource);
}
