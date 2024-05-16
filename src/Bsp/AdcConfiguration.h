/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

// clang-format off

#define Port
#define Bit
#define Channel

// Name, port, bit, channel
#define ADC_TABLE(ENTRY)         \
   ENTRY(Erd_BspAdc_ANALOG_475K_TH_00,   E, 3, 19) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_00,    E, 0, 16) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_01,    E, 5, 21) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_02,    D, 4, 28) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_03,    D, 3, 27) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_04,    4, 7, 7) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_05,    D, 2, 26) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_06,    4, 6, 6) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_07,    4, 5, 5) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_08,    4, 4, 4) \
   ENTRY(Erd_BspAdc_ANALOG_TH_LOW_09,    4, 1, 1) \
   ENTRY(Erd_BspAdc_LOAD_CURRENT,        4, 3, 3) \
   ENTRY(Erd_BspAdc_HW_PERSONALITY_00,   4, 2, 2)
// clang-format on

#endif
