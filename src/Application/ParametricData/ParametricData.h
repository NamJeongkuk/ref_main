/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARAMETRICDATA_H
#define PARAMETRICDATA_H

#include "ImageHeader.h"
#include "ParametricDataTableOfContents.h"

const ImageHeader_t *ParametricData_GetParametricHeader(void);

const ParametricDataTableOfContents_t *ParametricData_GetParametricTableOfContents(void);

#endif
