/*!
 * @file
 * @brief This captures the high level state of the Twist Ice Maker state machine.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef TWISTTRAYICEMAKERHIGHLEVELSTATE_H
#define TWISTTRAYICEMAKERHIGHLEVELSTATE_H

#include <stdint.h>

enum
{
   TwistTrayIceMakerHighLevelState_NormalRun = 0,
   TwistTrayIceMakerHighLevelState_ThermistorFault,
   TwistTrayIceMakerHighLevelState_Fault,
   TwistTrayIceMakerHighLevelState_FactoryDiags,
};
typedef uint8_t TwistTrayIceMakerHighLevelState_t;

#endif
