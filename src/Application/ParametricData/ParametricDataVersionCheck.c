/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ParametricDataVersionCheck.h"
#include "ParametricDataTableOfContents.h"
#include "ParametricData.h"

enum
{
   Critical_MajorVersion = 0,
   Critical_MinorVersion = 20
};

void ParametricDataVersionCheck_Init(I_Action_t *jumpToBootloaderAction)
{
   const ImageHeader_t *parametricHeader = ParametricData_GetParametricHeader();

   if(Critical_MajorVersion != parametricHeader->criticalMajorVersion ||
      Critical_MinorVersion != parametricHeader->criticalMinorVersion)
   {
      Action_Invoke(jumpToBootloaderAction);
   }
}
