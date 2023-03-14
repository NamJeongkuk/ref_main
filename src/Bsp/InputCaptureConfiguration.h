/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef INPUTCAPTURECONFIGURATION_H
#define INPUTCAPTURECONFIGURATION_H

// clang-format off

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

// clang-format on
#endif
