/*!
 * @file
 * @brief Dispensing Special Options.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DISPENSINGSPECIALOPTIONS_H
#define DISPENSINGSPECIALOPTIONS_H

#include <stdint.h>

enum
{
   DispensingSpecialOptions_None,
   DispensingSpecialOptions_SwitchPressedOnSecondaryBoardRequired,
   DispensingSpecialOptions_EmptyRequest = 0xFF,
};
typedef uint8_t DispensingSpecialOptions_t;

#endif
