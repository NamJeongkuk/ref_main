/*!
 * @file
 * @brief
 *
 * Copyright GE Appliances - Confidential - All rights reserved.
 */

#include "Constants_Binary.h"
#include "MotorDriverArbitrator.h"

static bool Motor0ControlRequestIsSet(void *context)
{
   MotorDriverArbitrator_t *instance = context;
   bool motor0ControlRequest;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motor0ControlRequestErd,
      &motor0ControlRequest);

   return motor0ControlRequest;
}

static bool Motor1ControlRequestIsSet(void *context)
{
   MotorDriverArbitrator_t *instance = context;
   bool motor1ControlRequest;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motor1ControlRequestErd,
      &motor1ControlRequest);

   return motor1ControlRequest;
}

static bool Motor0DriveEnableIsSet(void *context)
{
   MotorDriverArbitrator_t *instance = context;
   bool motor0DriveEnable;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motor0DriveEnableErd,
      &motor0DriveEnable);

   return motor0DriveEnable;
}

static bool Motor1DriveEnableIsSet(void *context)
{
   MotorDriverArbitrator_t *instance = context;
   bool motor1DriveEnable;

   DataModel_Read(
      instance->_private.dataModel,
      instance->_private.config->motor1DriveEnableErd,
      &motor1DriveEnable);

   return motor1DriveEnable;
}

static void OnMotor0ControlRequestChange(void *context, const void *args)
{
   MotorDriverArbitrator_t *instance = context;
   const bool *motor0ControlRequest = args;

   if(*motor0ControlRequest)
   {
      if(!Motor1DriveEnableIsSet(instance))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->motor0DriveEnableErd,
            set);
      }
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motor0DriveEnableErd,
         clear);

      if(Motor1ControlRequestIsSet(instance))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->motor1DriveEnableErd,
            set);
      }
   }
}

static void OnMotor1ControlRequestChange(void *context, const void *args)
{
   MotorDriverArbitrator_t *instance = context;
   const bool *motor1ControlRequest = args;

   if(*motor1ControlRequest)
   {
      if(!Motor0DriveEnableIsSet(instance))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->motor1DriveEnableErd,
            set);
      }
   }
   else
   {
      DataModel_Write(
         instance->_private.dataModel,
         instance->_private.config->motor1DriveEnableErd,
         clear);

      if(Motor0ControlRequestIsSet(instance))
      {
         DataModel_Write(
            instance->_private.dataModel,
            instance->_private.config->motor0DriveEnableErd,
            set);
      }
   }
}

void MotorDriverArbitrator_Init(
   MotorDriverArbitrator_t *instance,
   I_DataModel_t *dataModel,
   const MotorDriverArbitratorConfiguration_t *config)
{
   instance->_private.dataModel = dataModel;
   instance->_private.config = config;

   EventSubscription_Init(
      &instance->_private.motor0ControlRequestChangeSubscription,
      instance,
      OnMotor0ControlRequestChange);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->motor0ControlRequestErd,
      &instance->_private.motor0ControlRequestChangeSubscription);

   EventSubscription_Init(
      &instance->_private.motor1ControlRequestChangeSubscription,
      instance,
      OnMotor1ControlRequestChange);

   DataModel_Subscribe(
      instance->_private.dataModel,
      instance->_private.config->motor1ControlRequestErd,
      &instance->_private.motor1ControlRequestChangeSubscription);
}
