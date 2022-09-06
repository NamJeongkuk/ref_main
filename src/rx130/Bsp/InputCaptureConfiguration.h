/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTCAPTURECONFIGURATION_H
#define INPUTCAPTURECONFIGURATION_H

/*
 * ENTRY(inputName, port, bit, timer, channel, type (motor = 0, fan = 1))
 */
#define INPUT_CAPTURE_TABLE(ENTRY)                     \
   ENTRY(Erd_BspFanInputCapture_CAPT_0, E, 1, 4, C, 0) \
   ENTRY(Erd_BspFanInputCapture_CAPT_1, E, 3, 4, B, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_2, E, 2, 4, A, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_5, A, 4, 5, U, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_4, A, 6, 5, V, 1)
#endif
