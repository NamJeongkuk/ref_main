#include "Fsm.h"
#include "FoxFsm.h"
#include "utils.h"
#include "Constants_Binary.h"


static void State_A(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_B(Fsm_t *fsm, const FsmSignal_t signal, const void *data);
static void State_C(Fsm_t *fsm, const FsmSignal_t signal, const void *data);

// static void SetErdClockwiseFalse();
// static void SetErdCounterClockwiseFalse();

static void OnCWChanged(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);
   // TestingFsm_t *instance = context;
   // const bool *TestInput1 = args;

   // instance = instance;
   // TestInput1 = TestInput1;

}

static void OnCCWChanged(void *context, const void *args)
{
   IGNORE(context);
   IGNORE(args);
   // TestingFsm_t *instance = context;
   // const bool *TestInput1 = args;

   // instance = instance;
   // TestInput1 = TestInput1;
}

void FoxFsm_Init(
   FoxFsm_t *instance,
   const FoxFsmData_t *glue,
   I_DataModel_t *dataModel)
{
    // instance->foxfsmdata = data;
    instance->_private.dataModel = dataModel;
    instance->_private.foxfsmdata = glue;




   Fsm_Init(&instance->_private.objfsm, State_A);


   EventSubscription_Init(
      &instance->_private.cweventsub,
      instance,
      OnCWChanged);
      
      //Sub not supported error on execution
   DataModel_Subscribe(
      dataModel,
      instance->_private.foxfsmdata->clockwise,
      &instance->_private.cweventsub);

   EventSubscription_Init(
      &instance->_private.ccweventsub,
      instance,
      OnCCWChanged);
      
      //Sub not supported error on execution
   DataModel_Subscribe(
      dataModel,
      instance->_private.foxfsmdata->clockwise,
      &instance->_private.ccweventsub);


//    EventSubscription_Init(
//       &instance->_private.ccweventsub,
//       instance,
//       OnCCWChanged);
      
//    DataModel_Subscribe(
//       dataModel,
//       instance->_private.foxfsmdata.cclockwise,
//       &instance->_private.ccweventsub);

//    EventSubscription_Init(
//       &instance->_private.foxeventsub,
//       instance,
//       OnFoxFsmErdChanged);

//    DataModel_Subscribe(
//       dataModel,
//       instance->_private.foxfsmdata.clockwise,
//       &instance->_private.foxeventsub);

//    DataModel_Subscribe(
//       dataModel,
//       instance->_private.foxfsmdata.cclockwise,
//       &instance->_private.foxeventsub);

}

static void State_A(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         // SetFsmStateTo(instance, FSM_TEST_SIG_IDLE);
         break;
      case FoxFsm_B:
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
         // SetFsmStateTo(instance, FSM_TEST_SIG_IDLE);
         break;
      case FoxFsm_C:
         Fsm_Transition(fsm, State_C);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}


static void State_C(Fsm_t *fsm, const FsmSignal_t signal, const void *data)
{
   IGNORE(data);
   switch(signal)
   {
      case FSM_ENTRY:
         // Entry actions
         // SetFsmStateTo(instance, FSM_TEST_SIG_IDLE);
         break;
      case FoxFsm_A:
         Fsm_Transition(fsm, State_A);
         break;
      case FSM_EXIT:
         // Exit actions
         break;
   }
}