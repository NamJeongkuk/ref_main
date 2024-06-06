#include "Fsm.h"
#include "FoxFsm.h"
#include "utils.h"

static void State_Idle(Fsm_t *fsm, const FsmSignal_t signal, const void *data);


static void OnFoxFsmErdChanged(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);
   // TestingFsm_t *instance = context;
   // const bool *TestInput1 = args;

   // instance = instance;
   // TestInput1 = TestInput1;
}

void FoxFSM_Init(
   FoxFsm_t *instance,
   I_DataModel_t *dataModel)
{
   Fsm_Init(&instance->_private.objfsm, State_Idle);

   EventSubscription_Init(
      &instance->_private.foxeventsub,
      instance,
      OnFoxFsmErdChanged);

   DataModel_Subscribe(
      dataModel,
      foxfsmconf.clockwise,
      &instance->_private.foxeventsub);

   DataModel_Subscribe(
      dataModel,
      foxfsmconf.cclockwise,
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