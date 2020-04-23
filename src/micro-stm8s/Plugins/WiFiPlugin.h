/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef WIFIPLUGIN_H
#define WIFIPLUGIN_H

#include "I_TinyDataSource.h"
#include "I_TinyGea2Interface.h"

/*!
 * @param dataSource
 */
void WiFiPlugin_Init(I_TinyDataSource_t *dataSource, I_TinyGea2Interface_t *gea2Interface);

#endif
