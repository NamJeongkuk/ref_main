"use strict";

const delay = require("javascript-common").util.delay;
const { Console } = require("console");
const { msPerSec } = require("../support/constants");
const constants = require("../support/constants");

describe("Defrost", () => {
   beforeEach(async () => {
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   const defrostHsmState = {
      defrostHsmStatePowerUp: "DefrostHsmState_PowerUp",
      defrostHsmStateIdle: "DefrostHsmState_Idle",
      defrostHsmStatePrechillPrep: "DefrostHsmState_PrechillPrep",
      defrostHsmStatePrechill: "DefrostHsmState_Prechill",
      defrostHsmStatePostPrechill: "DefrostHsmState_PostPrechill",
      defrostHsmStateHeaterOnEntry: "DefrostHsmState_HeaterOnEntry",
      defrostHsmStateHeaterOnHeat: "DefrostHsmState_HeaterOnHeat",
      defrostHsmStateDwell: "DefrostHsmState_Dwell",
      defrostHsmStatePostDwell: "DefrostHsmState_PostDwell",
      defrostHsmStateDisabled: "DefrostHsmState_Disabled"
   };

   const compressorSpeed = {
      compressorSpeedOff: "CompressorSpeed_Off",
      compressorSpeedOn: "CompressorSpeed_On",
      compressorSpeedLow: "CompressorSpeed_Low",
      compressorSpeedMid: "CompressorSpeed_Mid",
      compressorSpeedHigh: "CompressorSpeed_High",
      compressorSpeedSuperHigh: "CompressorSpeed_SuperHigh"
   };

   const compressorSpeedConfig = {
      compressorSpeedConfigSingleSpeed: "CompressorSpeedConfig_SingleSpeed",
      compressorSpeedConfigVariableSpeed: "CompressorSpeedConfig_VariableSpeed"
   };

   const energyDemandLevel = {
      energyDemandLevelOff: "EnergyDemandLevel_Off",
      energyDemandLevelLow: "EnergyDemandLevel_Low",
      energyDemandLevelMaxLoadReduction: "EnergyDemandLevel_MaxLoadReduction",
      energyDemandLevelDelayDefrost: "EnergyDemandLevel_DelayDefrost"
   };

   const defrostState = {
      defrostStateIdle: "DefrostState_Idle",
      defrostStatePrechill: "DefrostState_Prechill",
      defrostStateHeaterOn: "DefrostState_HeaterOn",
      defrostStateDwell: "DefrostState_Dwell",
      defrostStateDisabled: "DefrostState_Disabled"
   };

   const defrostTimerCounterFsmState = {
      defrostTimerCounterFsmStateDisabled: "DefrostTimerCounterFsmState_Disabled",
      defrostTimerCounterFsmStateEnabled: "DefrostTimerCounterFsmState_Enabled"
   };

   const doorAccelerationFsmState = {
      doorAccelerationFsmStateDisabled: "DoorAccelerationFsmState_Disabled",
      doorAccelerationFsmStateEnabled: "DoorAccelerationFsmState_Enabled"
   };

   const defrostTimerIsSatisfiedMonitorFsmState = {
      defrostTimerIsSatisfiedMonitorFsmStateDisabled: "DefrostTimerIsSatisfiedMonitorFsmState_Disabled",
      defrostTimerIsSatisfiedMonitorFsmStateEnabled: "DefrostTimerIsSatisfiedMonitorFsmState_Enabled"
   };

   const valvePosition = {
      valvePositionA: "ValvePosition_A",
      valvePositionB: "ValvePosition_B",
      valvePositionC: "ValvePosition_C",
      valvePositionD: "ValvePosition_D",
      valvePositionE: "ValvePosition_E",
      valvePositionHome: "ValvePosition_Home"
   };

   const providedTheCompressorIsSingleSpeed = async () => {
      await rx130.write("Erd_CompressorSpeedConfig", compressorSpeedConfig.compressorSpeedConfigSingleSpeed);
   };

   const providedTheCompressorSpeedIs = async(speed) => {
      const vote = {
         speed: speed,
         care: true
      };
      await rx130.write("Erd_CompressorSpeed_ResolvedVote", vote);
   };

   const providedSabbathModeIs = async(state) => {
      await rx130.write("Erd_SabbathMode", state);
   };

   const providedTheLastFzDefrostWas = async(state) => {
      await rx130.write("Erd_FzDefrostWasAbnormal", state);
   };

   const providedTheLastFfDefrostWas = async(state) => {
      await rx130.write("Erd_FfDefrostWasAbnormal", state);
   };

   const providedTheDemandResponseLevelIs = async(level) => {
      await rx130.write("Erd_DemandResponseLevel", level);
   };

   const providedTheFilteredFreezerCabinetTemperatureIs = async (tempDegFx100) => {
      await rx130.write("Erd_Freezer_FilteredTemperature", tempDegFx100);
   };

   const providedTheEepromDefrostStateAtStartUpIs = async (state) => {
      await rx130.write("Erd_DefrostState", state);
      await delay(constants.oneSecondInMsec * 5);
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5); // waiting for board to reset; don't think can get accurate time after power up
   };

   const providedTheSealedSystemValveIsIn = async (position) => {
      await rx130.write("Erd_SealedSystemValvePosition", position);
   };

   const providedDefrostIsEnabledAndInIdleState = async () => {
      // this sets EEPROM ERD then writes to reset ERD, 5 seconds later, then it waits 5 seconds to give the board time to start up
      await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
      await providedTheFilteredFreezerCabinetTemperatureIs(300);

      await providedTheCompressorIsSingleSpeed();
      await providedTheCompressorSpeedIs(compressorSpeed.compressorSpeedOn);

      await providedTheSealedSystemValveIsIn(valvePosition.valvePositionA);

      await providedSabbathModeIs(constants.disabled);

      await providedTheLastFfDefrostWas(constants.abnormal);
      await providedTheLastFzDefrostWas(constants.normal);

      await providedTheDemandResponseLevelIs(energyDemandLevel.energyDemandLevelOff);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePowerUp);

      await afterPowerUpDelay();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
   };

   const theDefrostTimerCounterModulesShouldBe = async (state) => {
      if(state)
      {
         await theDefrostTimerCounterShouldBe(defrostTimerCounterFsmState.defrostTimerCounterFsmStateEnabled);
         await theDoorAccelerationShouldBe(doorAccelerationFsmState.doorAccelerationFsmStateEnabled);
         await theDefrostIsSatisfiedMonitorShouldBe(defrostTimerIsSatisfiedMonitorFsmState.defrostTimerIsSatisfiedMonitorFsmStateEnabled);
      }
      else
      {
         await theDefrostTimerCounterShouldBe(defrostTimerCounterFsmState.defrostTimerCounterFsmStateDisabled);
         await theDoorAccelerationShouldBe(doorAccelerationFsmState.doorAccelerationFsmStateDisabled);
         await theDefrostIsSatisfiedMonitorShouldBe(defrostTimerIsSatisfiedMonitorFsmState.defrostTimerIsSatisfiedMonitorFsmStateDisabled);
      }
   };

   const theDefrostHsmStateShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DefrostHsmState");
      expect(actual).toEqual(state);
   };

   const freshFoodDoorAccelerationShouldBe = async (acceleration) => {
      const actual = await rx130.read("Erd_DefrostFfDoorAccelerationCount");
      expect(actual).toEqual(acceleration);
   };

   const timerIsSatisfiedShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DefrostTimerIsSatisfied");
      expect(actual).toEqual(state);
   };

   const defrostTimerCountShouldBeAtLeast = async (count) => {
      const actual = await rx130.read("Erd_DefrostTimerCountInSeconds");
      expect(actual >= count).toBe(true);
   };

   const theDefrostTimerCounterShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DefrostTimerCounterFsmState");
      expect(actual).toEqual(state);
   };

   const theDoorAccelerationShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DoorAccelerationFsmState");
      expect(actual).toEqual(state);
   };

   const theDefrostIsSatisfiedMonitorShouldBe = async (state) => {
      const actual = await rx130.read("Erd_DefrostTimerIsSatisfiedMonitorFsmState");
      expect(actual).toEqual(state);
   };

   const leftHandFfDoorIs = async (state) => {
      await rx130.write("Erd_LeftHandFfDoorIsOpen", state);
   };

   const afterPowerUpDelay = async () => {
      await delay(5 * constants.msPerSec);
   };

   const waitTimeInSeconds = async (seconds) => {
      await delay(seconds * msPerSec);
   };

   it("should enter Idle state when in Idle before power cycle and FZ temp is normal", async () => {
      await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
      await providedTheFilteredFreezerCabinetTemperatureIs(300);
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePowerUp);

      await afterPowerUpDelay();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
   });

   it("should transition to Prechill Prep when defrost timer is satisfied at max time between defrosts", async () => {
      /*
      Parametric data must be modified so you're not waiting forever
      times used for this test are:

      defrost.lua
      fz_abnormal_run_time_in_minutes = 1 (60 seconds)
      max_time_between_defrosts_in_minutes = 2 (120 seconds)
      max_prechill_holdoff_time_after_defrost_timer_satisfied_in_seconds = 0 (no extra time waited if valve doesn't move into position)
      */

      await providedDefrostIsEnabledAndInIdleState();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(30);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 30
      await defrostTimerCountShouldBeAtLeast(30);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(90);
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should transition to Prechill Prep state earlier with door acceleration added to the defrost timer count", async () => {
      /*
      Parametric data must be modified so you're not waiting forever
      times used for this test are:

      defrost.lua
      fz_abnormal_run_time_in_minutes = 1 (60 seconds)
      max_time_between_defrosts_in_minutes = 2 (120 seconds)
      max_prechill_holdoff_time_after_defrost_timer_satisfied_in_seconds = 0 (no extra time waited if valve doesn't move into position)
      */

      await providedDefrostIsEnabledAndInIdleState();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFfDoorIs(constants.open);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(10);
      // door open for 10 seconds => door acceleration is 10 * 87 = 870
      // once count is > fz abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
      // count is now ~ 60 + 870 = 930
      // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });
});
