#ifndef FOXFSM_H
#define FOXFSM_H

#include <stdint.h>
#include "Erd.h"
#include "Fsm.h"
#include "SystemErds.h"
#include "FoxFsmState.h"

typedef struct
{
   Erd_t clockwise;
   Erd_t cclockwise;
   Erd_t printstate;
} FoxFsmConfig_t;

typedef struct
{
   struct
   {
      EventSubscription_t foxeventsub;
      FoxFsmConfig_t foxfsmconf;
      Fsm_t objfsm;
   } _private;

} FoxFsm_t;

void FoxFSM_Init(FoxFsm_t *fsm, I_DataModel_t *dataModel);

#endif