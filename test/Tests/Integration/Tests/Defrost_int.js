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

   const fanSpeed = {
      fanSpeedOff: "FanSpeed_Off",
      fanSpeedLow: "FanSpeed_Low",
      fanSpeedMedium: "FanSpeed_Medium",
      fanSpeedHigh: "FanSpeed_High",
      fanSpeedSuperHigh: "FanSpeed_SuperHigh",
      fanSpeedSuperLow: "FanSpeed_SuperLow"
   };

   const compressorState = {
      compressorStateOff: "CompressorState_Off",
      compressorStateOn: "CompressorState_On",
      compressorStateMinimumOnTime: "CompressorState_MinimumOnTime",
      compressorStateMinimumOffTime: "CompressorState_MinimumOffTime",
      compressorStateMinimumRunTime: "CompressorState_MinimumRunTime",
   };

   const providedTheCompressorIsSingleSpeed = async () => {
      await rx130.write("Erd_CompressorSpeedConfig", compressorSpeedConfig.compressorSpeedConfigSingleSpeed);
   };

   const providedTheCompressorSpeedIs = async (speed) => {
      const vote = {
         speed: speed,
         care: true
      };
      await rx130.write("Erd_CompressorSpeed_ResolvedVote", vote);
   };

   const providedSabbathModeIs = async (state) => {
      await rx130.write("Erd_SabbathMode", state);
   };

   const providedTheLastFreezerDefrostWas = async (state) => {
      await rx130.write("Erd_FreezerDefrostWasAbnormal", state);
   };

   const providedTheLastFreshFoodDefrostWas = async (state) => {
      await rx130.write("Erd_FreshFoodDefrostWasAbnormal", state);
   };

   const providedTheDemandResponseLevelIs = async (level) => {
      await rx130.write("Erd_DemandResponseLevel", level);
   };

   const providedTheFilteredFreezerCabinetTemperatureIs = async (tempDegFx100) => {
      await rx130.write("Erd_Freezer_FilteredTemperatureResolved", tempDegFx100);
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

   const freezerEvaporatorThermistorIsInvalid = async () => {
      await rx130.write("Erd_FreezerEvaporatorThermistorIsValid", false);
   };

   const freezerEvaporatorThermistorIsValid = async () => {
      await rx130.write("Erd_FreezerEvaporatorThermistorIsValid", true);
   };

   const defrostTimerCountIs = async (count) => {
      await rx130.write("Erd_Eeprom_DefrostTimerCountInSeconds", count);
   };

   const providedDefrostIsEnabledAndInIdleState = async () => {
      // this sets EEPROM ERD then writes to reset ERD, 5 seconds later, then it waits 5 seconds to give the board time to start up
      await freezerEvaporatorThermistorIsValid();
      await defrostTimerCountIs(0);
      await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
      await providedTheFilteredFreezerCabinetTemperatureIs(300);

      await providedTheCompressorIsSingleSpeed();
      await providedTheCompressorSpeedIs(compressorSpeed.compressorSpeedOn);
      await theCompressorStateIs(compressorState.compressorStateMinimumOnTime);

      await providedTheSealedSystemValveIsIn(valvePosition.valvePositionA);

      await providedSabbathModeIs(constants.disabled);

      await providedTheLastFreshFoodDefrostWas(constants.abnormal);
      await providedTheLastFreezerDefrostWas(constants.normal);

      await providedTheDemandResponseLevelIs(energyDemandLevel.energyDemandLevelOff);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePowerUp);

      await freezerEvaporatorThermistorIsValid();
      await afterPowerUpDelay();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
   };

   const theDefrostTimerCounterModulesShouldBe = async (state) => {
      if (state) {
         await theDefrostTimerCounterShouldBe(defrostTimerCounterFsmState.defrostTimerCounterFsmStateEnabled);
         await theDoorAccelerationShouldBe(doorAccelerationFsmState.doorAccelerationFsmStateEnabled);
         await theDefrostIsSatisfiedMonitorShouldBe(defrostTimerIsSatisfiedMonitorFsmState.defrostTimerIsSatisfiedMonitorFsmStateEnabled);
      }
      else {
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
      const actual = await rx130.read("Erd_DefrostFreshFoodDoorAccelerationCount");
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

   const leftHandFreshFoodDoorIs = async (state) => {
      await rx130.write("Erd_LeftHandFreshFoodDoorIsOpen", state);
   };

   const afterPowerUpDelay = async () => {
      await delay(5 * constants.msPerSec);
   };

   const waitTimeInSeconds = async (seconds) => {
      await delay(seconds * msPerSec);
   };

   const theFreezerDefrostResolvedSetpointIs = async (temperatureDegFx100) => {
      const vote = {
         temperature: temperatureDegFx100,
         care: true
      };
      await rx130.write("Erd_FreezerSetpoint_ResolvedVote", vote);
   };

   const theFreezerDefrostSetpointVoteShouldBe = async (temperatureDegFx100) => {
      const actual = await rx130.read("Erd_FreezerSetpoint_DefrostVote");
      expect(actual.temperature).toEqual(temperatureDegFx100);
      expect(actual.care).toEqual(true);
   };

   const theFreshFoodHeaterShouldBeVoted = async (state) => {
      const actual = await rx130.read("Erd_FreezerDefrostHeater_DefrostVote");
      expect(actual.state).toEqual(state);
      expect(actual.care).toEqual(true);
   };

   const theFreezerHeaterShouldBeVoted = async (state) => {
      const actual = await rx130.read("Erd_FreshFoodDefrostHeater_DefrostVote");
      expect(actual.state).toEqual(state);
      expect(actual.care).toEqual(true);
   };

   const theIceCabinetFanShouldBeVoted = async (speed) => {
      const actual = await rx130.read("Erd_IceCabinetFanSpeed_DefrostVote");
      expect(actual.state).toEqual(fanSpeed.speed);
      expect(actual.care).toEqual(true);
   };

   const theValvePositionShouldBeVotedFor = async (position) => {
      const actual = await rx130.read("Erd_ValvePosition_DefrostVote");
      expect(actual.position).toEqual(position);
      expect(actual.care).toEqual(true);
   };

   const theValvePositionShouldNotBeVotedFor = async () => {
      const actual = await rx130.read("Erd_ValvePosition_DefrostVote");
      expect(actual.care).toEqual(false);
   };

   const theCurrentDefrostIsNotFreshFoodOnly = async () => {
      await rx130.write("Erd_DefrostIsFreshFoodOnly", false);
   };

   const theCurrentDefrostIsFreshFoodOnly = async () => {
      await rx130.write("Erd_DefrostIsFreshFoodOnly", true);
   };

   const theFreshFoodSetpointShouldBeVotedWith = async (temperature) => {
      const actual = await rx130.read("Erd_FreshFoodSetpoint_DefrostVote");
      expect(actual.temperature).toEqual(temperature);
      expect(actual.care).toEqual(true);
   };

   const theFreshFoodSetpointShouldNotBeVotedFor = async () => {
      const actual = await rx130.read("Erd_FreshFoodSetpoint_DefrostVote");
      expect(actual.care).toEqual(false);
   };

   const requestToExtendDefrostShouldBeSent = async () => {
      const actual = await rx130.read("Erd_ExtendDefrostSignal");
      expect(actual).toEqual(1);
   };

   const requestToExtendDefrostShouldNotBeSent = async () => {
      const actual = await rx130.read("Erd_ExtendDefrostSignal");
      expect(actual).toEqual(0);
   };

   const requestToEnableDoorHoldoffShouldBeSent = async () => {
      const actual = await rx130.read("Erd_DefrostDoorHoldOffRequest");
      expect(actual).toEqual(true);
   };

   const theCompressorStateIs = async (state) => {
      await rx130.write("Erd_CompressorState", state);
   };

   const defrostIsInPrechillPrep = async () => {
      await providedDefrostIsEnabledAndInIdleState(); // valve is in position C, it doesn't match three_way_valve_position_to_extend_defrost_with_fresh_food_cycle_defrost
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

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
   };

   /* Parametric data for the following tests
      defrost.lua
      freezer_abnormal_run_time_in_minutes = 1 (60 seconds)
      max_time_between_defrosts_in_minutes = 2 (120 seconds)
      max_prechill_holdoff_time_after_defrost_timer_satisfied_in_seconds = 0
      prechill_freezer_setpoint_in_degfx100 = -600
      three_way_valve_position_for_max_prechill_holdoff = three_way_valve_position_type.position_B,
      three_way_valve_position_to_extend_defrost_with_fresh_food_cycle_defrost = three_way_valve_position_type.position_B
      prechill_fresh_food_setpoint_in_degfx100 = 4600
      evaporator.lua
      number_evaporators = 1
   */

   it("should enter Idle state when in Idle before power cycle and FZ temp is normal", async () => {
      await providedTheEepromDefrostStateAtStartUpIs(defrostState.defrostStateIdle);
      await providedTheFilteredFreezerCabinetTemperatureIs(300);
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePowerUp);

      await afterPowerUpDelay();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateIdle);
   });

   it("should transition to Prechill Prep when defrost timer is satisfied at max time between defrosts", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(30);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 30
      await defrostTimerCountShouldBeAtLeast(30);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(95);
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);

      //Once prechill compressor on time is satisfied, go to prechill state
      await theCompressorStateIs(compressorState.compressorStateOn);
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechill);
   });

   it("should transition to Prechill Prep state earlier with door acceleration added to the defrost timer count", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(10);
      // door open for 10 seconds => door acceleration is 10 * 87 = 870
      // once count is > freezer abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
      // count is now ~ 60 + 870 = 930
      // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should transition to Heater On Entry state when FZ evap thermistor is invalid upon entry to Prechill Prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsInvalid();
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
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStateHeaterOnEntry);
   });

   it("should vote for resolved freezer set point on entry if resolved freezer set point is less than prechill freezer set point to prechill prep if single evap", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-700);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(10);
      // door open for 10 seconds => door acceleration is 10 * 87 = 870
      // once count is > freezer abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
      // count is now ~ 60 + 870 = 930
      // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theFreezerDefrostSetpointVoteShouldBe(-700);
      await theFreshFoodHeaterShouldBeVoted(constants.off);
      await theFreezerHeaterShouldBeVoted(constants.off);
      await theIceCabinetFanShouldBeVoted(fanSpeed.fanSpeedHigh);
   });

   it("should vote for prechill freezer set point on entry if resolved freezer set point is equal to than prechill freezer set point to prechill prep if single evap", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-600);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(10);
      // door open for 10 seconds => door acceleration is 10 * 87 = 870
      // once count is > freezer abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
      // count is now ~ 60 + 870 = 930
      // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theFreezerDefrostSetpointVoteShouldBe(-600);
      await theFreshFoodHeaterShouldBeVoted(constants.off);
      await theFreezerHeaterShouldBeVoted(constants.off);
      await theIceCabinetFanShouldBeVoted(fanSpeed.fanSpeedHigh);
   });

   it("should vote for prechill freezer set point on entry if resolved freezer set point is greater than prechill freezer set point to prechill prep if single evap", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

      // once count reaches max time between defrosts in seconds (120 seconds or greater)
      // timer is satisfied should be set
      await waitTimeInSeconds(10);
      // door open for 10 seconds => door acceleration is 10 * 87 = 870
      // once count is > freezer abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
      // count is now ~ 60 + 870 = 930
      // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
      // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
      // so you can't capture when it actually is set

      // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theFreezerDefrostSetpointVoteShouldBe(-600);
      await theFreshFoodHeaterShouldBeVoted(constants.off);
      await theFreezerHeaterShouldBeVoted(constants.off);
      await theIceCabinetFanShouldBeVoted(fanSpeed.fanSpeedHigh);
   });


   it("should not vote for valve position if max prechill holdoff time is equal to zero on entry to prechill prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

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
      await theValvePositionShouldNotBeVotedFor();
   });

   it("should vote for fresh food setpoint if current defrost is not fresh food only on entry to prechill prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);
      await theCurrentDefrostIsNotFreshFoodOnly();

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

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
      await theFreshFoodSetpointShouldBeVotedWith(4600); // matches prechill_fresh_food_setpoint_in_degfx100
   });
   it("should not vote for fresh food setpoint if current defrost is fresh food only on entry to prechill prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);
      await theCurrentDefrostIsFreshFoodOnly();

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

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
      await theFreshFoodSetpointShouldNotBeVotedFor();
   });

   it("should request to extend defrost if current valve position is in position to extend defrost on entry to prechill prep", async () => {
      await providedDefrostIsEnabledAndInIdleState();
      await freezerEvaporatorThermistorIsValid();
      await theDefrostTimerCounterModulesShouldBe(constants.enabled);
      await theFreezerDefrostResolvedSetpointIs(-400);

      // increment defrost counter by waiting some amount of time
      await waitTimeInSeconds(50);
      // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
      await defrostTimerCountShouldBeAtLeast(50);

      // door opening
      await leftHandFreshFoodDoorIs(constants.open);

      await await providedTheSealedSystemValveIsIn(valvePosition.valvePositionB); // matches three_way_valve_position_to_extend_defrost_with_fresh_food_cycle_defrost

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
      await requestToExtendDefrostShouldBeSent();
   });

   it("should not request to extend defrost if current valve position is not in position to extend defrost on entry to prechill prep", async () => {
      await defrostIsInPrechillPrep();

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await requestToExtendDefrostShouldNotBeSent();
   });

   it("should request to enable door holdoff on entry to prechill prep", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await requestToEnableDoorHoldoffShouldBeSent();
   });

   it("should transition to post prechill from prechill prep when last defrost changes from normal to abnormal", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await providedTheLastFreezerDefrostWas(constants.abnormal);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePostPrechill);
   });

   it("should transition to prechill from prechill prep when compressor run time is satisfied", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theCompressorStateIs(compressorState.compressorStateOn);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechill);
   });

   it("should not transition from prechill prep when compressor state is minimum off time", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theCompressorStateIs(compressorState.compressorStateMinimumOffTime);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should not transition from prechill prep when compressor state is minimum on time", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theCompressorStateIs(compressorState.compressorStateMinimumOnTime);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   it("should not transition from prechill prep when compressor state is minimum run time", async () => {
      await defrostIsInPrechillPrep();
      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
      await theCompressorStateIs(compressorState.compressorStateMinimumRunTime);

      await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   });

   /* Parametric data for the following tests
      defrost.lua
      freezer_abnormal_run_time_in_minutes = 1 (60 seconds)
      max_time_between_defrosts_in_minutes = 2 (120 seconds)
      max_prechill_holdoff_time_after_defrost_timer_satisfied_in_seconds = 0
      prechill_freezer_setpoint_in_degfx100 = -600
      three_way_valve_position_for_max_prechill_holdoff = three_way_valve_position_type.position_B,
      three_way_valve_position_to_extend_defrost_with_fresh_food_cycle_defrost = three_way_valve_position_type.position_B
      prechill_fresh_food_setpoint_in_degfx100 = 4600
      evaporator.lua
      number_evaporators = 2
   */

   // it("should vote for prechill freezer setpoint and turn off defrost heaters and turn ice cabinet fan on high on entry to prechill prep if dual evap", async () => {
   //    await providedDefrostIsEnabledAndInIdleState();
   //    await freezerEvaporatorThermistorIsValid();
   //    await theDefrostTimerCounterModulesShouldBe(constants.enabled);

   //    // increment defrost counter by waiting some amount of time
   //    await waitTimeInSeconds(50);
   //    // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
   //    await defrostTimerCountShouldBeAtLeast(50);

   //    // door opening
   //    await leftHandFreshFoodDoorIs(constants.open);

   //    // once count reaches max time between defrosts in seconds (120 seconds or greater)
   //    // timer is satisfied should be set
   //    await waitTimeInSeconds(10);
   //    // door open for 10 seconds => door acceleration is 10 * 87 = 870
   //    // once count is > fz abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
   //    // count is now ~ 60 + 870 = 930
   //    // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
   //    // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
   //    // so you can't capture when it actually is set

   //    // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
   //    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   //    await theFreezerDefrostSetpointVoteShouldBe(-600);
   //    await theFreshFoodHeaterShouldBeVoted(constants.off);
   //    await theFreezerHeaterShouldBeVoted(constants.off);
   //    await theIceCabinetFanShouldBeVoted(fanSpeed.fanSpeedHigh);
   // });


   // /* Parametric data for the following tests
   //    defrost.lua
   //    freezer_abnormal_run_time_in_minutes = 1 (60 seconds)
   //    max_time_between_defrosts_in_minutes = 2 (120 seconds)
   //    max_prechill_holdoff_time_after_defrost_timer_satisfied_in_seconds = 30
   //    prechill_freezer_setpoint_in_degfx100 = -600
   //    three_way_valve_position_for_max_prechill_holdoff = three_way_valve_position_type.position_B,
   //    three_way_valve_position_to_extend_defrost_with_fresh_food_cycle_defrost = three_way_valve_position_type.position_B
   //    prechill_fresh_food_setpoint_in_degfx100 = 4600
   //    evaporator.lua
   //    number_evaporators = 1
   // */

   // it("should vote for valve position if max prechill holdoff time is greater than zero on entry to prechill prep", async () => {
   //    await providedDefrostIsEnabledAndInIdleState();
   //    await freezerEvaporatorThermistorIsValid();
   //    await theDefrostTimerCounterModulesShouldBe(constants.enabled);
   //    await theFreezerDefrostResolvedSetpointIs(-400);

   //    // increment defrost counter by waiting some amount of time
   //    await waitTimeInSeconds(50);
   //    // there's some unknown time between reset ERD being written and board coming back up so I'm checking that the count is changing and at least 50
   //    await defrostTimerCountShouldBeAtLeast(50);

   //    // door opening
   //    await leftHandFreshFoodDoorIs(constants.open);

   //    // once count reaches max time between defrosts in seconds (120 seconds or greater)
   //    // timer is satisfied should be set
   //    await waitTimeInSeconds(10 + 30); // 30 for the max holdoff time
   //    // door open for 10 seconds => door acceleration is 10 * 87 = 870
   //    // once count is > fz abnormal run time (but still less than max time between defrosts), it'll add the door acceleration to the count
   //    // count is now ~ 60 + 870 = 930
   //    // once count is > max time between defrosts, it'll set defrost timer is satisfied (930 > 120)
   //    // after it's set, defrost will exit Idle which also causes the timer is satisfied flag to be cleared
   //    // so you can't capture when it actually is set

   //    // defrost should transition to prechill prep after the defrost timer is satisfied flag is set
   //    await theDefrostHsmStateShouldBe(defrostHsmState.defrostHsmStatePrechillPrep);
   //    await theValvePositionShouldBeVotedFor(valvePosition.valvePositionB);
   // });
});
