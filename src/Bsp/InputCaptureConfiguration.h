/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTCAPTURECONFIGURATION_H
#define INPUTCAPTURECONFIGURATION_H

// clang-format off

#ifdef OLD_HW
/*
 * ENTRY(inputName, port, bit, timer, channel, type (motor = 0, fan = 1))
 */
#define INPUT_CAPTURE_TABLE(ENTRY)                     \
   ENTRY(Erd_BspFanInputCapture_CAPT_0, E, 1, 4, C, 0) \
   ENTRY(Erd_BspFanInputCapture_CAPT_1, E, 3, 4, B, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_2, E, 2, 4, A, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_4, A, 6, 5, V, 1) \
   ENTRY(Erd_BspFanInputCapture_CAPT_5, A, 4, 5, U, 1)
#else
/*
 * ENTRY(inputName,                     port, bit, timer, channel
 */
#define INPUT_CAPTURE_TABLE(ENTRY)                          \
   ENTRY(Erd_BspInputCapture_CAPTURE_00, D,   7,   5,     U) \
   ENTRY(Erd_BspInputCapture_CAPTURE_01, D,   6,   5,     V) \
   ENTRY(Erd_BspInputCapture_CAPTURE_02, D,   5,   5,     W) \
   ENTRY(Erd_BspInputCapture_CAPTURE_03, B,   3,   4,     A) \
   ENTRY(Erd_BspInputCapture_CAPTURE_04, 2,   5,   4,     C) \
   ENTRY(Erd_BspInputCapture_CAPTURE_05, 2,   6,   2,     A)
#endif

// clang-format on
#endif
