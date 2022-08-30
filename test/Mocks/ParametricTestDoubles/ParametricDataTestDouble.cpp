/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "ParametricDataTestDouble.h"
#include <iostream>
#include <sstream>
#include <stdbool.h>
#include <stdint.h>
#include "uassert_test.h"
#include "LuaObj.h"

using namespace std;

enum
{
   NumberOfPersonalities = 1,
   DirectoryOfPersonality = 0,
   ScriptNameOfPersonality = 1,
   DirectoryAndScriptName = 2,
   DefaultPersonalityForTest = Personality_TddDevelopment,
};

static const string filePathByPersonality[NumberOfPersonalities][DirectoryAndScriptName] = {
   { "Parametric/data/Development/Personality/", "personalityTestDrivenDevelopmentSingleEvap.lua" }
};

static bool loaded[NumberOfPersonalities];
static long applicationParametricData[NumberOfPersonalities][10000];

void *GivenThatTheApplicationParametricDataHasBeenLoadedIntoAPointer(PersonalityId_t personalityForTest)
{
   stringstream script;

   if(!loaded[personalityForTest])
   {
      script << "package.path = package.path .. ';Parametric/src/?.lua;Parametric/lib/?/?.lua';";
      script << "local env = (require 'Environment')('" << filePathByPersonality[personalityForTest][DirectoryOfPersonality].c_str() << "');";
      script << "local loaded_file, err = loadfile('" << filePathByPersonality[personalityForTest][DirectoryOfPersonality].c_str() << filePathByPersonality[personalityForTest][ScriptNameOfPersonality].c_str() << "', 't', env);";
      script << "return '<data>' .. tostring(loaded_file()) .. env.core.attic() .. '</data>'";
      uassert(LuaObj_Generate(script.str().c_str(), applicationParametricData[personalityForTest], sizeof(applicationParametricData[personalityForTest]), true));
      loaded[personalityForTest] = true;
   }

   return &applicationParametricData[personalityForTest][0];
}
