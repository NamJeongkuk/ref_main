#include "Erd_t"
#include "Fsm.h"

enum
{
   FSM_TEST_INIT = Fsm_UserSignalStart,
   FSM_TEST_SIG_IDLE,
   FSM_TEST_SIG_A,
   FSM_TEST_SIG_B,
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_A(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_B(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         break;
      case FSM_TEST_SIG_IDLE:
         Fsm_Transition(fsm, State_A);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}
static void State_A(struct Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         break;
      case FSM_TEST_SIG_A:
         Fsm_Transition(fsm, State_B);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}
static void State_B(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         break;
      case FSM_TEST_SIG_B:
         Fsm_Transition(fsm, State_Idle);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}

void TestingFSM_Init(Fsm_t *fsm);