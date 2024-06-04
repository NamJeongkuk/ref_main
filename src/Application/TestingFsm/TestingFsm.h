#ifndef TESTINGFSM_H
#define TESTINGFSM_H

#include <stdint.h>
#include "Erd.h"
#include "Fsm.h"
#include "SystemErds.h"
#include "TestingState.h"
// #include

typedef struct
{
   Erd_t clockwise;
   Erd_t cclockwise;
   Erd_t printstate;
} TestingFsmConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t foxeventsub;
      Fsm_t objfsm;
   } _private;

} TestingFsm_t;

void TestingFSM_Init(TestingFsm_t *fsm, I_DataModel_t *dataModel);

#endif