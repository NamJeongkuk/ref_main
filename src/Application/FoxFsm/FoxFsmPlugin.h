#ifndef FOXFSMPLUGIN_H
#define FOXFSMPLUGIN_H

#include "I_DataModel.h"
#include "FoxFsm.h"

typedef struct {
    FoxFsm_t foxfsm;
} FoxFsmPlugin_t ;

//#define FoxFsmPlugin_t FoxFsm_t;

void  FoxFsmPlugin_Init(
   FoxFsmPlugin_t *instance,
   I_DataModel_t *dataModel);



#endif