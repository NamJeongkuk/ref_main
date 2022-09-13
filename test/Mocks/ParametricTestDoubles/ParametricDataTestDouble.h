/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef PARAMETRICDATATESTDOUBLE_H
#define PARAMETRICDATATESTDOUBLE_H

#include "PersonalityId.h"

enum
{
   Personality_TddDevelopment,
   Personality_Max
};

/*!
 * @param personalityForTest Requested personality for this test
 */
void *GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(PersonalityId_t personalityForTest);

#endif