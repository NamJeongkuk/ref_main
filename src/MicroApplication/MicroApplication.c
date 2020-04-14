/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "MicroApplication.h"
#include "MicroSystemErds.h"

void MicroApplication_Init(MicroApplication_t *instance, I_TinyDataSource_t *dataSource)
{
   ApplianceApiPlugin_Init(&instance->_private.ApplianceApiPlugin, dataSource);
}
