// #include "Erd_t"
#include "Fsm.h"
#include "TestingFsm.h"
#include "utils.h"

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_A(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_B(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

void TestingFSM_Init(TestingFsm_t *Testing)
{
   // Testing->_private.objfsm = objfsm;
   Fsm_Init(&Testing->_private.objfsm, State_Idle);
}

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         // SetFsmStateTo(instance, FSM_TEST_SIG_IDLE);
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
   IGNORE(data);
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
   IGNORE(data);
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         break;
      case FSM_TEST_SIG_B:
         Fsm_Transition(fsm, State_A);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}
