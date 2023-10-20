/*!
 * @file
 * @brief Defines read / write callback functions for ERDs converted datasource
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef CONVERTEDCALLBACKS_H
#define CONVERTEDCALLBACKS_H

/*!
 * The empty write callback function.
 * This callback should be assigned for write callback of read only ERDs
 * @param context The pointer of data source
 * @param dataToWrite Received data to write
 */
void ErdConvertedCallBack_WriteNotAvailable(void *context, const void *dataToWrite);

/*!
 * The read callback function to read the first 8 bytes of the Unit Serial Number for the RFID filter
 * @pre context != null
 * @pre dataToFill != null
 * @param context The pointer of data source
 * @param dataToFill Read buffer to fill
 */
void ErdConvertedCallBack_ReadRfidFilterUnitSerialNumber(void *context, void *dataToFill);

#endif
