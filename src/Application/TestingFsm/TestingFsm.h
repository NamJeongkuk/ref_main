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
   Erd_t sampleerd;
   Erd_t sampleerd2;
} TestingFsmConfig_t;

// void AugerMotorController_Init(
//    AugerMotorController_t *instance,
//    I_DataModel_t *dataModel,
//    const AugerMotorControllerConfig_t *config,
//    const AugerMotorData_t *augerMotorData);

// typedef struct
// {
//    struct
//    {
//       I_DataModel_t *dataModel;
//       EventSubscription_t augerMotorIceTypeVoteChangeSubscription;
//       const AugerMotorData_t *augerMotorData;
//       const AugerMotorControllerConfig_t *config;
//       AugerMotorIceType_t storedAugerMotorIceType;
//       Fsm_t fsm;
//       Timer_t timer;
//    } _private;
// }  AugerMotorController_t;

typedef struct
{
   struct
   {
      EventSubscription_t foxeventsub;
      TestingFsmConfig_t *tconfig;
      Fsm_t objfsm;
   } _private;

} TestingFsm_t;

void TestingFSM_Init(TestingFsm_t *fsm, I_DataModel_t *dataModel);

#endif