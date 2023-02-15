/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARAMETRICDATA_TESTDOUBLE_H
#define PARAMETRICDATA_TESTDOUBLE_H

extern "C"
{
#include "ImageHeader.h"
#include "ParametricDataTableOfContents.h"
}

const ParametricDataTableOfContents_t *ParametricData_GetParametricTableOfContents();

void SetImageHeaderCrc(uint16_t crc);
void SetImageHeaderMajorVersion(uint16_t majorVersion);
void SetImageHeaderMinorVersion(uint16_t minorVersion);

void PersonalityParametricData_TestDouble_Init();

#endif
