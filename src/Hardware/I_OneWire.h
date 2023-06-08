/*!
 * @file
 * @brief Low level interface for one wire device
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#ifndef I_ONEWIRE_H
#define I_ONEWIRE_H

#include <stdint.h>
#include <stdbool.h>

struct I_OneWire_Api_t;

typedef struct
{
   const struct I_OneWire_Api_t *api;
} I_OneWire_t;

typedef struct I_OneWire_Api_t
{
   /*!
    * Read from the device
    * @param instance The instance of the device
    * @param address The offset to read from
    * @param numBytes The number of bytes to read
    * @param readBuffer The buffer to read the data into
    * @return number of bytes read (0 means read failed)
    */
   uint8_t (*Read)(I_OneWire_t *instance, uint8_t address, uint8_t numBytes, uint8_t *readBuffer);

   /*!
    * Write to the device
    * @param instance The instance of the device
    * @param address The offset to start writing
    * @param numBytes The number of bytes to write
    * @param writeBuffer The buffer from which the data will be written
    * @return number of bytes written (0 means read failed)
    */
   uint8_t (*Write)(I_OneWire_t *instance, uint8_t address, uint8_t numBytes, const uint8_t *writeBuffer);

   /*!
    * Identify that the device is present
    * @param instance The instance of the device
    * @return TRUE if device is present
    */
   bool (*DeviceIsPresent)(I_OneWire_t *instance);
} I_OneWire_Api_t;

#define OneWire_Read(instance, address, numBytes, readBuffer) \
   (instance)->api->Read((instance), (address), (numBytes), (readBuffer))

#define OneWire_Write(instance, address, numBytes, writeBuffer) \
   (instance)->api->Write((instance), (address), (numBytes), (writeBuffer))

#define OneWire_DeviceIsPresent(instance) \
   (instance)->api->DeviceIsPresent((instance))

#endif
