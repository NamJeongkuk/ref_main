#ifndef FOXFSMSTATE_H
#define FOXFSMSTATE_H


typedef enum {
   FoxFsm_ENTRY = Fsm_Entry,
   FoxFsm_EXIT = Fsm_Exit,
   FoxFsm_IDLE,
   FoxFsm_A,
   FoxFsm_B,
   FoxFsm_C
} FoxFsmState_t;

#endif