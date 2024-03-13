/*!
 * @file
 * @brief Run a test with a matrix of personalities
 *
 * Usage:
 * #define PERSONALITIES(ENTRY) \
 *   ENTRY(Blah) \
 *   ENTRY(Blah2)
 *
 * TEST_GROUP(Boo)
 *   setup(PersonalityId_t personality)
 *
 * ... some tests
 *
 * #undef PERSONALITIES
 * #define PERSONALITIES(ENTRY) \
 *  ENTRY(Blah3) \
 *  ENTRY(Blah3)
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#pragma once

#include "TestMatrix.hpp"

#undef TEST
#define TEST(testGroup, testName) \
   MATRIX_TEST(testGroup, testName, PERSONALITIES)
