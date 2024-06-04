// #include "Erd_t"
#include "Fsm.h"
#include "TestingFsm.h"
#include "utils.h"

TestingFsmConfig_t namtestconfig = {
   .clockwise = Erd_Clockwise,
   .cclockwise = Erd_CClockwise,
   .printstate = Erd_TestCurState
};

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_A(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_B(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

static void OnTestErd1Changed(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);
   // TestingFsm_t *instance = context;
   // const bool *TestInput1 = args;

   // instance = instance;
   // TestInput1 = TestInput1;
}

static void OnTestErd2Changed(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);
   // TestingFsm_t *instance = context;
   // const bool *TestInput2 = args;

   // instance = instance;
   // TestInput2 = TestInput2;
}

void TestingFSM_Init(
   TestingFsm_t *instance,
   I_DataModel_t *dataModel)
{
   Fsm_Init(&instance->_private.objfsm, State_Idle);

   EventSubscription_Init(
      &instance->_private.foxeventsub,
      instance,
      OnTestErd1Changed);

   DataModel_Subscribe(
      dataModel,
      namtestconfig.clockwise,
      &instance->_private.foxeventsub);

   EventSubscription_Init(
      &instance->_private.foxeventsub,
      instance,
      OnTestErd2Changed);

   DataModel_Subscribe(
      dataModel,
      namtestconfig.cclockwise,
      &instance->_private.foxeventsub);
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
