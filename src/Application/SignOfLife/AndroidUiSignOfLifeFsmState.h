/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ANDROIDUISIGNOFLIFEFSMSTATE_H
#define ANDROIDUISIGNOFLIFEFSMSTATE_H

#include <stdint.h>

enum
{
   AndroidUiSignOfLifeFsmState_HasCommunicationWithUi,
   AndroidUiSignOfLifeFsmState_LostCommunicationWithUi,
   AndroidUiSignOfLifeFsmState_Fault,
   AndroidUiSignOfLifeFsmState_Idle,
};
typedef uint8_t AndroidUiSignOfLifeFsmState_t;

#endif
