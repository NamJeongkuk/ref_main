/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef REFRIGERATORMODEL_H
#define REFRIGERATORMODEL_H

#include "MajorConfigurationType.h"
#include "RefrigeratorModelType.h"

typedef struct
{
   RefrigeratorModelType_t refrigeratorModelType;
   uint8_t applicationId;
   MajorConfigurationType_t majorConfigurationType;
} RefrigeratorModel_t;

#endif
