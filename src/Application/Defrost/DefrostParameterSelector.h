/*!
 * @file
 * @brief Determines and stores the following times into ERDs based on various conditions:
 * - max prechill time
 * - post dwell exit time
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTPARAMETERSELECTOR_H
#define DEFROSTPARAMETERSELECTOR_H

#include "I_DataModel.h"

void DefrostParameterSelector_Init(I_DataModel_t *dataModel);

#endif
