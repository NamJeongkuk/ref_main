/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef MAJORCONFIGURATIONTYPE_H
#define MAJORCONFIGURATIONTYPE_H

#include <stdint.h>

enum
{
   MajorConfigurationType_SingleEvaporatorVariableSpeedCompSEVS,
   MajorConfigurationType_DualEvaporatorSingleSpeedCompDESS,
   MajorConfigurationType_DualEvaporatorVariableSpeedCompDEVS,
   MajorConfigurationType_SingleEvaporatorSingleSpeedCompSESS,
   MajorConfigurationType_TripleEvaporatorVariableSpeedCompTEVS,
   MajorConfigurationType_TripleEvaporatorSingleSpeedCompTESS
};
typedef uint8_t MajorConfigurationType_t;

#endif
