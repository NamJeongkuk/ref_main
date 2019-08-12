/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef ADCCONFIGURATION_H
#define ADCCONFIGURATION_H

enum
{
   AdcChannel_0,
   AdcChannel_1,
   AdcChannel_2,
   AdcChannel_3,
   AdcChannel_4,
   AdcChannel_5,
   AdcChannel_6,
   AdcChannel_7,
   AdcChannel_8,
   AdcChannel_9,
   AdcChannel_10,
   AdcChannel_11,
   AdcChannel_12,
   AdcChannel_13,
   AdcChannel_14,
   AdcChannel_15,
   AdcChannel_16,
   AdcChannel_17,
   AdcChannel_18,
   AdcChannel_19,
   AdcChannel_20,
   AdcChannel_21,
   AdcChannel_22,
   AdcChannel_23,
   AdcChannel_24,
   AdcChannel_25,
   AdcChannel_26,
   AdcChannel_27,
   AdcChannel_28,
   AdcChannel_29,
   AdcChannel_30,
   AdcChannel_31,

   // Virtual ADC channels for Self Diagnostics
   AdcChannel_SelfDiagnostic_Vref0,
   AdcChannel_SelfDiagnostic_VrefHalf,
   AdcChannel_SelfDiagnostic_VrefFull,

   AdcChannel_Unused,
   AdcChannel_Max = AdcChannel_Unused,

   AdcChannel_NumberOfSelfDiagnosticChannels = 3
};

#endif
