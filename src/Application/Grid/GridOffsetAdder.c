/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "GridOffsetAdder.h"

void GridOffsetAdder_Init(
   GridOffsetAdder_t *instance,
   I_DataModel_t *dataModel,
   const GridOffsetAdderErdConfiguration_t *config)
{
   I16ErdAdder_Init(
      &instance->_private.I16ErdAdder,
      dataModel,
      config->I16ErdAdderConfig);
}
