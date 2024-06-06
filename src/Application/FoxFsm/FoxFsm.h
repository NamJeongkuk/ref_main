#ifndef FOXFSM_H
#define FOXFSM_H

#include <stdint.h>
#include "Erd.h"
#include "Fsm.h"
#include "SystemErds.h"
#include "FoxFsmState.h"
#include "I_DataModel.h"

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
      I_DataModel_t *dataModel;
      EventSubscription_t cweventsub;
      EventSubscription_t ccweventsub;
      FoxFsmData_t foxfsmdata;
      Fsm_t objfsm;
   } _private;

} FoxFsm_t;

void FoxFsm_Init(
   FoxFsm_t *instance,
   const FoxFsmData_t *glue,
   I_DataModel_t *dataModel);

   

#endif