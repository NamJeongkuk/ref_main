#ifndef TESTINGSTATE_H
#define TESTINGSTATE_H

#include <stdint.h>
#include "Fsm.h"

typedef enum
{
   FSM_TEST_INIT = Fsm_Entry,
   FSM_TEST_EXIT = Fsm_Exit,
   FSM_TEST_SIG_IDLE = Fsm_UserSignalStart,
   FSM_TEST_SIG_A,
   FSM_TEST_SIG_B,
} TestFsmState_t;

#endif