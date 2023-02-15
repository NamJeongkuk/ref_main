/*!
 * @file
 * @brief Matches Defrost HSM state to overall Defrost State which is saved in EEPROM
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef DEFROSTSTATEONCOMPAREMATCH_H
#define DEFROSTSTATEONCOMPAREMATCH_H

#include "I_DataModel.h"
#include "ErdWriterOnCompareMatch.h"

typedef struct
{
   struct
   {
      ErdWriterOnCompareMatch_t erdWriterOnCompareMatch;
   } _private;
} DefrostStateOnCompareMatch_t;

/*!
 *
 * @param instance
 * @param dataModel
 */
void DefrostStateOnCompareMatch_Init(DefrostStateOnCompareMatch_t *instance, I_DataModel_t *dataModel);

#endif
