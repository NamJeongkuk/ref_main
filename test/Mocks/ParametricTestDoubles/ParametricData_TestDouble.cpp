/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

extern "C"
{
#include "ParametricData.h"
#include "ParametricDataTableOfContents.h"
}
#include "ParametricData_TestDouble.h"

enum
{
   PersonalityId_Max = 3
};

static PersonalityParametricData_t *personalities[PersonalityId_Max];
static ParametricDataTableOfContents_t parametricData = {
   .shortGitHash = { { 0x12, 0x34, 0x56, 0x78 } },
   .numberOfPersonalities = PersonalityId_Max,
   .personalities = personalities
};

static ImageHeader_t imageHeader;

typedef struct
{
   ImageHeader_t *imageHeader;
   ParametricDataTableOfContents_t *parametricData;
} PersonalityParametric_TestDouble_t;

static PersonalityParametric_TestDouble_t instance;

extern "C" const ImageHeader_t *ParametricData_GetParametricHeader(void)
{
   return instance.imageHeader;
}

const ParametricDataTableOfContents_t *ParametricData_GetParametricTableOfContents()
{
   return instance.parametricData;
}

void SetImageHeaderCrc(uint16_t crc)
{
   instance.imageHeader->imageCrc = crc;
}

void SetImageHeaderMajorVersion(uint16_t majorVersion)
{
   instance.imageHeader->criticalMajorVersion = majorVersion;
}

void SetImageHeaderMinorVersion(uint16_t minorVersion)
{
   instance.imageHeader->criticalMinorVersion = minorVersion;
}

void PersonalityParametricData_TestDouble_Init()
{
   instance.parametricData = &parametricData;
   instance.imageHeader = &imageHeader;
}
