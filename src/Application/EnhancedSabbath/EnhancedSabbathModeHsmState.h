/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ENHANCEDSABBATHMODEHSMSTATE_H
#define ENHANCEDSABBATHMODEHSMSTATE_H

#include <stdint.h>

enum
{
   EnhancedSabbathModeHsmState_Disabled,
   EnhancedSabbathModeHsmState_Stage_FreshFood,
   EnhancedSabbathModeHsmState_Stage_Freezer,
   EnhancedSabbathModeHsmState_Stage_Defrosting,
   EnhancedSabbathModeHsmState_Stage_Off,
};
typedef uint8_t EnhancedSabbathModeHsmState_t;

#endif
