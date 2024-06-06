#ifndef FOXFSMSTATE_H
#define FOXFSMSTATE_H

#include <stdint.h>
#include "Fsm.h"

enum {
   FoxFsm_ENTRY = Fsm_Entry,
   FoxFsm_EXIT = Fsm_Exit,
   FoxFsm_A = Fsm_UserSignalStart,
   FoxFsm_B,
   FoxFsm_C
};
typedef uint16_t FoxFsmState_t;

#endif