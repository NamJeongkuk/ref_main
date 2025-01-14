/*!
 * @file
 * @brief Fan control data which allows both open loop (DutyCycle) and closed loop (RPM) requests.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef FANCONTROL_H
#define FANCONTROL_H

typedef uint16_t FanRpm_t;

#define SWAP_DEFINITION_FanRpm_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

typedef uint16_t FanDutyCycle_t;

#define SWAP_DEFINITION_FanDutyCycle_t(_name, _type) \
   SWAPPED_ERD(_name, sizeof(_type)),

enum
{
   FanControlType_DutyCycle,
   FanControlType_Rpm
};
typedef uint8_t FanControlType_t;

typedef struct
{
   FanControlType_t type;

   union
   {
      FanDutyCycle_t dutyCycle;
      FanRpm_t rpm;
   };
} FanControl_t;

#define SWAP_DEFINITION_FanControl_t(_name, _type) \
   SWAPPED_FIELD(_name, _type, dutyCycle),

#endif
