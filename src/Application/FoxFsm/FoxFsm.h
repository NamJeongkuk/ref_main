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
} FoxFsmData_t;

typedef struct
{
   struct
   {
      EventSubscription_t foxeventsub;
      FoxFsmData_t foxfsmdata;
      Fsm_t objfsm;
   } _private;

} FoxFsm_t;

void FoxFsm_Init(
   FoxFsm_t *instance,
   I_DataModel_t *dataModel);

#endif