/*!
 * @file
 * @brief Some helpful RX231 Defines
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef RX231DEFINES_H
#define RX231DEFINES_H

#define U16_PRCR_ENABLE                   ((uint16_t)0xA50F)
#define U16_PRCR_DISABLE                  ((uint16_t)0xA500)

#define ENABLE_PRCR_PRC1_ACCESS_MASK      ((uint16_t)0xA502)
#define ENTER_STOP_MODE                   (1)
#define EXIT_STOP_MODE                    (0)

#define USE_PIN_AS_GENERAL_IO (0)
#define USE_PIN_AS_PERIPHERAL (1)

#define DISABLE_MPC_WRITE_PROTECT() \
      MPC.PWPR.BIT.B0WI = 0;    \
      MPC.PWPR.BIT.PFSWE = 1;   \

#define ENABLE_MPC_WRITE_PROTECT() \
      MPC.PWPR.BIT.B0WI = 1;   \
      MPC.PWPR.BIT.PFSWE = 0;  \

#define SetPortDirection(PORT, DIRECTION_MASK) \
   PORT.PDR.BYTE = DIRECTION_MASK \

#define SetPortData(PORT, BITMASK, VALUE)  \
   PORT.PODR.BYTE = (PORT.PODR.BYTE & ~BITMASK) | (BITMASK & VALUE) \

#define SetPortPullups(PORT, PULLUPS) \
   PORT.PCR.BYTE = PULLUPS \

#define ConfigPort(PORT, INITIAL_VALUE, INPUT_BITMASK, PULLUP_BITMASK) \
   SetPortData(PORT, 0xFF, INITIAL_VALUE); \
   SetPortDirection(PORT, (0xFF & ~INPUT_BITMASK)); \
   SetPortPullups(PORT, PULLUP_BITMASK);\

#endif
