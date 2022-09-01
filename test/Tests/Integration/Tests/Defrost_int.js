"use strict";

const delay = require("javascript-common").util.delay;
const { Console } = require("console");
const { msPerSec } = require("../support/constants");
const constants = require("../support/constants");

describe("Defrost", () => {
   const defrostHsmState = {
      defrostHsmStatePowerUp: "DefrostHsmState_PowerUp",
      defrostHsmStateIdle: "DefrostHsmState_Idle",
      defrostHsmStatePrechillPrep: "DefrostHsmState_PrechillPrep",
      defrostHsmStatePrechill: "DefrostHsmState_Prechill",
      defrostHsmStateHeaterOnEntry: "DefrostHsmState_HeaterOnEntry",
      defrostHsmStateHeaterOn: "DefrostHsmState_HeaterOn",
      defrostHsmStateDwell: "DefrostHsmState_Dwell",
      defrostHsmStatePostDwell: "DefrostHsmState_PostDwell",
      defrostHsmStateDisabled: "DefrostHsmState_Disabled"
   };

   const defrostState = {
      defrostStateIdle: "DefrostState_Idle",
      defrostStatePrechill: "DefrostState_Prechill",
      defrostStateHeaterOn: "DefrostState_HeaterOn",
      defrostStateDwell: "DefrostState_Dwell",
      defrostStateDisabled: "DefrostState_Disabled"
   };

   const compressorState = {
      compressorStateOffAndReadyToChange: "CompressorState_OffAndReadyToChange",
      compressorStateOnAndReadyToChange: "CompressorState_OnAndReadyToChange",
      compressorStateMinimumOnTime: "CompressorState_MinimumOnTime",
      compressorStateMinimumOffTime: "CompressorState_MinimumOffTime",
      compressorStateVariableSpeedMinimumRunTime: "CompressorState_VariableSpeedMinimumRunTime",
   };

   const powerUpDelayInMsec = 5 * constants.msPerSec;

   //minimum_time_between_defrosts_abnormal_run_time_in_minutes * seconds_per_minute
   //6 * 60 * 60 = 21600
   const minimumDefrostReadySatisfactionTimeInMinutes = 21600;
   const minimumDefrostReadyOnTimeInSec = minimumDefrostReadySatisfactionTimeInMinutes * 60;

   const somePrechillConditionTimeInMinutes = 3;
   const someTemperatureThatLiesInTheMiddleOfTheBounds = 4000;
   const someLowerTemperatureBound = 0;
   const defrostHeaterOnDelayAfterCompressorOffInSeconds = 2;

   beforeEach(async () => {
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const providedTheFilteredFreezerCabinetTemperatureIs = async (tempDegFx100) => {
      await rx130.write("Erd_Freezer_FilteredTemperatureResolved", tempDegFx100);
   };

   const providedTheEepromDefrostStateAtStartUpIs = async (state) => {
      await rx130.write("Erd_DefrostState", state);
      await after(powerUpDelayInMsec).inMsec();
      await rx130.write("Erd_Reset", 2);
      await after(powerUpDelayInMsec).inMsec();
   };

   const providedDefrostIsEnabledAndInIdleState = async () => {
      await providedFreezerEvaporatorThermistorIs().valid();
      await providedTheFilteredFreezerCabinetTemperatureIs(300);
      await theCompressorStateIs(compressorState.compressorStateMinimumOnTime);

      await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
   };

   const providedTheDefrostReadySatisfactionTimeIs = async (timeInMinutes) => {
      await rx130.write("Erd_TimeInMinutesWhenDefrostReadyTimerIsSatisfied", timeInMinutes);
   };

   const theDefrostHsmStateShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DefrostHsmState");
      expect(actual).toEqual(state);
   };

   const theCompressorStateIs = async (state) => {
      await rx130.write("Erd_CompressorState", state);
   };

   const after = (time) => ({
      inMsec: async () => {
         await delay(time);
      },
      inSec: async () => {
         await delay(time * msPerSec);
      }
   });

   const providedThereWasNoPreviousAbnormalDefrost = async () => {
      await rx130.write("Erd_FreezerDefrostWasAbnormal", false);
      await rx130.write("Erd_FreshFoodDefrostWasAbnormal", false);
      await rx130.write("Erd_ConvertibleCompartmentDefrostWasAbnormal", false);
   };

   const providedFreezerEvaporatorThermistorIs = () => ({
      valid: async () => {
         await rx130.write("Erd_FreezerEvapThermistor_IsValidOverrideRequest", true);
         await rx130.write("Erd_FreezerEvapThermistor_IsValidOverrideValue", true);
      },
      invalid: async () => {
         await rx130.write("Erd_FreezerEvapThermistor_IsValidOverrideValue", false);
         await rx130.write("Erd_FreezerEvapThermistor_IsValidOverrideRequest", false);
      }
   });

   const providedFreshFoodEvaporatorThermistorIs = () => ({
      valid: async () => {
         await rx130.write("Erd_FreshFoodEvapThermistor_IsValidOverrideRequest", true);
         await rx130.write("Erd_FreshFoodEvapThermistor_IsValidOverrideValue", true);
      },
      invalid: async () => {
         await rx130.write("Erd_FreshFoodEvapThermistor_IsValidOverrideValue", false);
         await rx130.write("Erd_FreshFoodEvapThermistor_IsValidOverrideRequest", false);
      }
   });

   const providedFreshFoodThermistorIs = () => ({
      valid: async () => {
         await rx130.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", true);
         await rx130.write("Erd_FreshFoodThermistor_IsValidOverrideValue", true);
      },
      invalid: async () => {
         await rx130.write("Erd_FreshFoodThermistor_IsValidOverrideValue", false);
         await rx130.write("Erd_FreshFoodThermistor_IsValidOverrideRequest", false);
      }
   });

   const providedTheCompressor = () => ({
      isOn: async () => {
         await rx130.write("Erd_CompressorIsOn", true);
      },
      isOff: async () => {
         await rx130.write("Erd_CompressorIsOn", false);
      }
   });

   const providedCompartmentWasNotTooWarmOnPowerUp = async () => {
      await rx130.write("Erd_FreezerFilteredTemperatureTooWarmOnPowerUpReady", false);
   };

   const providedTimeThatPrechillConditionsAreMetIs = async (timeInMinutes) => {
      await rx130.write("Erd_TimeThatPrechillConditionsAreMetInMinutes", timeInMinutes);
   };

   const whenTheCompressorHasBeenOnFor = (timeInSec) => ({
      seconds: async () => {
         await rx130.write("Erd_DefrostCompressorOnTimeInSeconds", timeInSec);
      }
   });

   const whenTheDoorAccelerationTimeForThe = () => ({
      FreshFoodCompartmentIs: async (timeInSec) => {
         await rx130.write("Erd_DefrostFreshFoodScaledDoorAccelerationInSeconds", timeInSec);
      },
      FreezerCompartmentIs: async (timeInSec) => {
         await rx130.write("Erd_DefrostFreezerScaledDoorAccelerationInSeconds", timeInSec);
      },
      ConvertibleCompartmentIs: async (timeInSec) => {
         await rx130.write("Erd_DefrostConvertibleCompartmentScaledDoorAccelerationInSeconds", timeInSec);
      }
   });

   const providedTheFilteredResolvedTemperatureFor = () => ({
      TheFreshFoodThermistorIs: async (temperature) => {
         await rx130.write("Erd_FreshFood_FilteredTemperatureResolved", temperature);
      },
      TheFreezerThermistorIs: async (temperature) => {
         await rx130.write("Erd_Freezer_FilteredTemperatureResolved", temperature);
      },
      TheFreezerEvapThermistorIs: async (temperature) => {
         await rx130.write("Erd_FreezerEvap_FilteredTemperatureResolved", temperature);
      }
   });

   const theVoteFor = () => ({
      CompressorSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rx130.read("Erd_CompressorSpeed_DefrostVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      },
      FreezerFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rx130.read("Erd_FreezerFanSpeed_DefrostVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      },
      CondenserFanSpeedShouldBe: async (expectedCare, expectedSpeed = null) => {
         const actual = await rx130.read("Erd_CondenserFanSpeed_DefrostVote");
         if (expectedSpeed != null) {
            expect(actual.speed).toEqual(expectedSpeed);
         }
         expect(actual.care).toEqual(expectedCare);
      },
      DamperPositionShouldBe: async (expectedCare, expectedPosition = null) => {
         const actual = await rx130.read("Erd_FreshFoodDamperPosition_DefrostVote");
         if (expectedPosition != null) {
            expect(actual.position).toEqual(expectedPosition);
         }
         expect(actual.care).toEqual(expectedCare);
      }
   });

   it("should enter Idle state when in Idle before power cycle and FZ temp is normal", async () => {
      await providedDefrostIsEnabledAndInIdleState();
   });

   it("should transition to Prechill Prep when defrost timer is satisfied at max time between defrosts", async () => {
      await providedDefrostIsEnabledAndInIdleState();

      await providedFreshFoodEvaporatorThermistorIs().valid();
      await providedFreezerEvaporatorThermistorIs().valid();
      await providedFreshFoodThermistorIs().valid();
      await providedThereWasNoPreviousAbnormalDefrost();
      await providedCompartmentWasNotTooWarmOnPowerUp();
      await providedTheCompressor().isOff();

      await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
      await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should transition to Prechill Prep state earlier with door acceleration added to the defrost timer count", async () => {
      await providedDefrostIsEnabledAndInIdleState();

      await providedThereWasNoPreviousAbnormalDefrost();
      await providedCompartmentWasNotTooWarmOnPowerUp();
      await providedFreshFoodThermistorIs().valid();
      await providedTheCompressor().isOff();
      await providedFreshFoodEvaporatorThermistorIs().valid();
      await providedFreezerEvaporatorThermistorIs().valid();

      await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes * 3);
      await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();
      await whenTheDoorAccelerationTimeForThe().FreshFoodCompartmentIs(minimumDefrostReadyOnTimeInSec);
      await whenTheDoorAccelerationTimeForThe().FreezerCompartmentIs(minimumDefrostReadyOnTimeInSec);
      await whenTheDoorAccelerationTimeForThe().ConvertibleCompartmentIs(minimumDefrostReadyOnTimeInSec);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should transition to heater on entry state when FZ evap thermistor is invalid upon entry to Prechill Prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();

      await providedThereWasNoPreviousAbnormalDefrost();
      await providedCompartmentWasNotTooWarmOnPowerUp();
      await providedFreshFoodThermistorIs().valid();
      await providedTheCompressor().isOff();
      await providedFreshFoodEvaporatorThermistorIs().valid();
      await providedFreezerEvaporatorThermistorIs().invalid();

      await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
      await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);

      await theVoteFor().CompressorSpeedShouldBe(true, "CompressorSpeed_Off");
      await theVoteFor().FreezerFanSpeedShouldBe(true, "FanSpeed_Off");
      await theVoteFor().CondenserFanSpeedShouldBe(true, "FanSpeed_Off");
   });

   it("should transition from prechill prep to prechill and vote loads", async () => {
      await providedDefrostIsEnabledAndInIdleState();

      await providedThereWasNoPreviousAbnormalDefrost();
      await providedCompartmentWasNotTooWarmOnPowerUp();
      await providedFreshFoodThermistorIs().valid();
      await providedTheCompressor().isOn();
      await providedFreshFoodEvaporatorThermistorIs().valid();
      await providedFreezerEvaporatorThermistorIs().valid();

      await providedTimeThatPrechillConditionsAreMetIs(somePrechillConditionTimeInMinutes);

      await providedTheFilteredResolvedTemperatureFor().TheFreshFoodThermistorIs(someTemperatureThatLiesInTheMiddleOfTheBounds);
      await providedTheFilteredResolvedTemperatureFor().TheFreezerThermistorIs(someLowerTemperatureBound);
      await providedTheFilteredResolvedTemperatureFor().TheFreezerEvapThermistorIs(someLowerTemperatureBound);

      await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
      await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechill);

      await theVoteFor().CompressorSpeedShouldBe(true, "CompressorSpeed_On");
      await theVoteFor().FreezerFanSpeedShouldBe(true, "FanSpeed_SuperLow");
      await theVoteFor().DamperPositionShouldBe(true, "DamperPosition_Closed");
   });

   it("should transition from heater on entry to heater on after defrost heater on delay after compressor is off", async () => {
      await providedDefrostIsEnabledAndInIdleState();

      await providedThereWasNoPreviousAbnormalDefrost();
      await providedCompartmentWasNotTooWarmOnPowerUp();
      await providedFreshFoodThermistorIs().valid();
      await providedTheCompressor().isOff();
      await providedFreshFoodEvaporatorThermistorIs().valid();
      await providedFreezerEvaporatorThermistorIs().invalid();

      await providedTheDefrostReadySatisfactionTimeIs(minimumDefrostReadySatisfactionTimeInMinutes);
      await whenTheCompressorHasBeenOnFor(minimumDefrostReadyOnTimeInSec).seconds();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);

      await after(defrostHeaterOnDelayAfterCompressorOffInSeconds).inSec();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOn);

   });
});
