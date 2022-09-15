/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "LuaParametricTestUtils.h"
#include "PersonalityId.h"
#include "LuaObj.h"
#include "uassert_test.h"
#include <iostream>
#include <sstream>

using namespace std;

static long applicationParametricData[10000];

void GivenDataHasBeenGeneratedFor(const char *lua)
{
   stringstream script;

   script << "package.path = package.path .. ';Parametric/src/?.lua;Parametric/lib/?/?.lua';";
   script << "local env = (require 'Environment')('');";
   script << "local loaded = load([==[return " << lua << "]==], 'parametric', 't', env);";
   script << "return '<data>' .. tostring(loaded()) .. env.core.attic() .. '</data>'";

   uassert(LuaObj_Generate(script.str().c_str(), applicationParametricData, sizeof(applicationParametricData), true));
}

const void *ParametricData()
{
   return reinterpret_cast<const void *>(applicationParametricData);
}
