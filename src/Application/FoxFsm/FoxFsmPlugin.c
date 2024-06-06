#include "SystemErds.h"
#include "FoxFsmPlugin.h"
#include "FoxFsm.h"

FoxFsmData_t foxtestconfigfortdd = {
   .clockwise = Erd_Clockwise,
   .cclockwise = Erd_CClockwise,
   .printstate = Erd_TestCurState
};




void  FoxFsmPlugin_Init(
   FoxFsmPlugin_t *instance,
   I_DataModel_t *dataModel)

{
   FoxFsm_Init(&instance->foxfsm, dataModel);

}
