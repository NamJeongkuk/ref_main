#include <stdint.h>
#include "Fsm.h"
#include "TestingState.h"

typedef struct
{
   struct
   {
      uint8_t counter;
      Fsm_t objfsm;
   } _private;

} TestingFsm_t;

void TestingFSM_Init(TestingFsm_t *fsm);