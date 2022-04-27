/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CCCABINETSTATEPLUGIN_H
#define CCCABINETSTATEPLUGIN_H

#include "CcCabinetState.h"
#include "CcCabinetData.h"
#include "I_DataModel.h"

typedef struct
{
   CcCabinetState_t instance;
} CcCabinetStatePlugin_t;

void CcCabinetStatePlugin_Init(CcCabinetStatePlugin_t *instance, I_DataModel_t *dataModel);

#endif
