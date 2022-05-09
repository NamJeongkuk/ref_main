/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTREQUEST_H
#define DEFROSTREQUEST_H

enum
{
   DefrostRequest_Idle,
   DefrostRequest_Disable,
   DefrostRequest_Enable,
   DefrostRequest_ForceFreshFoodPrechill,
   DefrostRequest_ForceFreshFoodAndFreezerPrechill,
   DefrostRequest_AhamSetUpForFreshFoodPrechill,
   DefrostRequest_AhamSetUpForFreshFoodAndFreezerPrechill
};
typedef uint8_t DefrostRequest_t;

#endif
