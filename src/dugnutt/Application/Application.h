/*!
 * @file
 * @brief Application Glue Code
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "I_DataModel.h"

typedef struct
{
   struct
   {
      uint8_t dummy;
   } _private;
} Application_t;

/*!
 * Initialize Application
 * @param instance
 * @param dataModel
 */
void Application_Init(
   Application_t *instance,
   I_DataModel_t *dataModel);

#endif
