/*!
 * @file
 * @brief Parametric data for inhibit dispensing when door opens
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DOORINHIBITDISPENSETABLE_H
#define DOORINHIBITDISPENSETABLE_H

typedef struct
{
   bool leftHandFreshFood;
   bool rightHandFreshFood;
   bool leftHandFreezer;
   bool rightHandFreezer;
   bool convertibleCompartment;
   bool doorInDoor;
} DoorInhibitDispenseTable_t;

#endif
