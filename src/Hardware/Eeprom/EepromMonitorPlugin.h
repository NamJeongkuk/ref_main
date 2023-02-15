/*!
 * @file
 * @brief Monitors Eeprom Read/Write/Erase status and updates Fault ERDs
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef EEPROMMONITORPLUGIN_H
#define EEPROMMONITORPLUGIN_H

#include "I_DataModel.h"

/*!
 * Initialize the eeprom fault monitor.
 * @param dataModel The DataModel
 */
void EepromMonitorPlugin_Init(I_DataModel_t *dataModel);

#endif
