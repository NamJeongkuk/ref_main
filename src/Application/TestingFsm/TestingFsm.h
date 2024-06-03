#include <stdint.h>
#include "Erd.h"
#include "Fsm.h"
#include "TestingState.h"

typedef struct
{
   Erd_t sampleerd;
   Erd_t sampleerd2;
} TestingFsmConfig_t;

typedef struct
{
   struct
   {
      TestingFsmConfig_t tconfig;
      Fsm_t objfsm;
   } _private;

} TestingFsm_t;

// TestingFsmConfig_t testconfig = {
// .sampleerd  = ;
// .sampleerd2 = ;
// };

void TestingFSM_Init(TestingFsm_t *fsm);