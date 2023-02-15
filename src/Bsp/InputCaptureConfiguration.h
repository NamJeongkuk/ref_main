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
 * peripheralFunction is the value to write to MPC.PXnPFS.BIT.PSEL, see hardware manual
 * ENTRY(inputName,                     port, bit, timer, channel, peripheralFunction)
 */
#define INPUT_CAPTURE_TABLE(ENTRY)                          \
   ENTRY(Erd_BspInputCapture_CAPTURE_00, D,   7,   5,     U,       1) \
   ENTRY(Erd_BspInputCapture_CAPTURE_01, D,   6,   5,     V,       1) \
   ENTRY(Erd_BspInputCapture_CAPTURE_02, D,   5,   5,     W,       1) \
   ENTRY(Erd_BspInputCapture_CAPTURE_03, B,   3,   4,     A,       2) \
   ENTRY(Erd_BspInputCapture_CAPTURE_04, 2,   5,   4,     C,       1) \
   ENTRY(Erd_BspInputCapture_CAPTURE_05, 2,   6,   2,     A,       1)
#endif

// clang-format on
#endif
