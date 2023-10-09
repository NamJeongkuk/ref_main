/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RFIDCOMMUNICATIONCONTROLLERSTATE_H
#define RFIDCOMMUNICATIONCONTROLLERSTATE_H

#include <stdint.h>

enum
{
   RfidCommunicationControllerHsmState_Active,
   RfidCommunicationControllerHsmState_AllFreshFoodDoorsJustClosed,
   RfidCommunicationControllerHsmState_FreshFoodDoorOpen,
   RfidCommunicationControllerHsmState_DemoMode,
   RfidCommunicationControllerHsmState_AllFreshFoodDoorsClosed,
   RfidCommunicationControllerHsmState_AllDoorsClosedRead,
   RfidCommunicationControllerHsmState_AllDoorsClosedWrite
};
typedef uint8_t RfidCommunicationControllerState_t;

#endif
