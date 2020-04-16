/*!
 * @file
 * @brief Defines cap sense keys for a given target.
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CAPSENSECONFIGURATION_H
#define CAPSENSECONFIGURATION_H

enum
{
   CapSensePollPeriodMsec = 5
};

#define CAP_SENSE_CONFIGURATION(ENTRY) \
   ENTRY(CapSense_U102, 102)           \
   ENTRY(CapSense_U103, 103)           \
   ENTRY(CapSense_U104, 104)           \
   ENTRY(CapSense_U105, 105)           \
   ENTRY(CapSense_U106, 106)           \
   ENTRY(CapSense_U107, 107)           \
   ENTRY(CapSense_U108, 108)           \
   ENTRY(CapSense_U109, 109)           \
   ENTRY(CapSense_U110, 110)           \
   ENTRY(CapSense_U111, 111)           \
   ENTRY(CapSense_U112, 112)           \
   ENTRY(CapSense_U113, 113)           \
   ENTRY(CapSense_U114, 114)

#endif
