"use strict";

const delay = require("javascript-common").util.delay;
const constants = require("../support/constants");
const After = require('../support/After.js');
const { msPerSec } = require("../support/constants");

/*
Change the following ERDs to NvUnitSetting in SystemErds.h:

Erd_AluminumMoldIceMakerMoldThermistorIsValidOverrideValue
Erd_AluminumMoldIceMakerMoldThermistorIsValidOverrideRequest
Erd_AluminumMoldIceMakerRakePosition
Erd_IceMakerEnabledOverrideRequest
Erd_IceMakerEnabledOverrideValue

Flash board with DEBUG=Y for time acceleration to work
*/

describe("AluminumMoldIceMaker,", () => {
   const aLittleBitMoreThanAMinuteAccelerationDelayInSeconds = 65;
   const aLittleBitAccelerationDelayInSeconds = 10;

   const after = (time) => ({
      inMsec: async () => {
         await After(time);
      },
      inSec: async () => {
         await After(time * constants.msPerSec);
      },
      inMin: async () => {
         await After(time * constants.msPerMin);
      }
   });

   const hsmState = {
      global: "AluminumMoldIceMakerHsmState_Global",
      freeze: "AluminumMoldIceMakerHsmState_Freeze",
      idleFreeze: "AluminumMoldIceMakerHsmState_IdleFreeze",
      harvest: "AluminumMoldIceMakerHsmState_Harvest",
      fill: "AluminumMoldIceMakerHsmState_Fill",
      harvestFix: "AluminumMoldIceMakerHsmState_HarvestFix",
      harvestFault: "AluminumMoldIceMakerHsmState_HarvestFault",
      thermistorFault: "AluminumMoldIceMakerHsmState_ThermistorFault"
   };

   const waterValveState = {
      off: "WaterValveState_Off",
      on: "WaterValveState_On"
   };

   const relayState = {
      off: "RelayState_Off",
      on: "RelayState_On"
   };

   const motorState = {
      off: "MotorState_Off",
      on: "MotorState_On"
   };

   const percentageDutyCycle = {
      min: "PercentageDutyCycle_Min",
      max: "PercentageDutyCycle_Max"
   };

   const voteCare = {
      dontCare: "Vote_DontCare",
      care: "Vote_Care"
   };

   const rakePosition = {
      notHome: "RakePosition_NotHome",
      home: "RakePosition_Home"
   };

   const iceMakerTestRequest = {
      none: "IceMakerTestRequest_None",
      fill: "IceMakerTestRequest_Fill",
      harvest: "IceMakerTestRequest_Harvest"
   }

   // Parametric values
   const freezeThawFillTubeHeaterOnTimeInSeconds = 420;
   const freezeThawFillTubeHeaterDutyCyclePercentage = 100;
   const maximumHarvestTimeInMinutes = 7;
   const heaterOnTemperatureInDegFx100 = 3200;
   const heaterOffTemperatureInDegFx100 = 5000;
   const initialMinimumHeaterOnTimeInSeconds = 30;
   const timedIceMakerFillInSecondsx10 = 31;

   const theAluminumMoldIceMakerHsmStateShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldIceMakerHsmState");
      expect(actual).toEqual(expected);
   };

   const sabbathModeIsDisabled = async () => {
      await rockhopper.write("Erd_SabbathMode", false);
   };

   const sabbathModeIsEnabled = async () => {
      await rockhopper.write("Erd_SabbathMode", true);
   };

   const iceMakerIsEnabled = async () => {
      await rockhopper.write("Erd_IceMakerEnabledOverrideRequest", true);
      await rockhopper.write("Erd_IceMakerEnabledOverrideValue", true);
   };

   const iceMakerIsDisabled = async () => {
      await rockhopper.write("Erd_IceMakerEnabledOverrideRequest", true);
      await rockhopper.write("Erd_IceMakerEnabledOverrideValue", false);
   };

   const resetBoard = async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   };

   const theRakePositionIsHome = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerRakePosition", rakePosition.home);
   };

   const theRakePositionIsNotHome = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerRakePosition", rakePosition.notHome);
   };

   const providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState = async (state) => {
      await theMoldThermistorIsValid();
      await theRakePositionIsHome();

      if (state == hsmState.freeze) {
         await sabbathModeIsEnabled();
         await iceMakerIsDisabled();
      }
      else if (state == hsmState.IdleFreeze) {
         await sabbathModeIsDisabled();
         await iceMakerIsEnabled();
      }
      else if (state == hsmState.idleFreeze) {
         await sabbathModeIsEnabled();
         await iceMakerIsEnabled();
      }
      else if (state == hsmState.harvest) {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsNotReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsReadyToEnterHarvest();

         await harvestCountIsReadyToHarvest();
      }
      else if (state == hsmState.fill) {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
         await theRakeCompletesAFullRevolution();
         await skipFillRequestIsClear();

         await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
         await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      }
      else if (state == hsmState.harvestFix) {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

         await after(maximumHarvestTimeInMinutes - 1).inMin();
         await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      }
      await theAluminumMoldIceMakerHsmStateShouldBe(state);
   };

   const providedTheAluminumMoldIceMakerIsInFreezeState = () => ({
      andWaitingForHarvestCountToBeReadyForHarvest: async () => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsNotReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsReadyToEnterHarvest();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      },
      andWaitingForFeelerArmToBeReadyToEnterHarvest: async () => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsNotReadyToEnterHarvest();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      }
   });

   const providedTheAluminumIceMakerIsInIdleFreeze = () => ({
      andSabbathModeIsEnabled: async () => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

         await sabbathModeIsEnabled();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
      },
      andIceMakerIsDisabled: async () => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

         await iceMakerIsDisabled();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
      }
   });

   const theIceMakerWaterValveShouldBeVoted = async (state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerWaterValve_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(voteCare.care);
   };

   const theIsolationWaterValveShouldBeVoted = async (state) => {
      const vote = await rockhopper.read("Erd_IsolationWaterValve_AluminumMoldIceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(voteCare.care);
   };

   const theIsolationWaterValveShouldBeVotedDontCare = async (state) => {
      const vote = await rockhopper.read("Erd_IsolationWaterValve_AluminumMoldIceMakerVote");
      expect(vote.care).toEqual(voteCare.dontCare);
   };

   const theIceMakerHeaterShouldBeVoted = async (state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerHeaterRelay_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(voteCare.care);
   };

   const theIceMakerMotorShouldBeVoted = async (state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerRakeMotor_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(voteCare.care);
   };

   const harvestCountCalculationRequestShouldBe = async (state) => {
      const actual = await rockhopper.read("Erd_HarvestCountCalculationRequest");
      expect(actual).toEqual(state);
   };

   const feelerArmMonitoringRequestShouldBe = async (state) => {
      const actual = await rockhopper.read("Erd_FeelerArmMonitoringRequest");
      expect(actual).toEqual(state);
   };

   const harvestCountIsNotReadyToHarvest = async () => {
      await rockhopper.write("Erd_HarvestCountIsReadyToHarvest", false);
   };

   const harvestCountIsReadyToHarvest = async () => {
      await rockhopper.write("Erd_HarvestCountIsReadyToHarvest", true);
   };

   const iceMakerTemperatureIsReadyToHarvest = async () => {
      let temperature = 1900 - 1; // max harvest temperature - 1
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100", temperature);
   };

   const iceMakerTemperatureIsNotReadyToHarvest = async () => {
      let temperature = 1900; // max harvest temperature
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideRequest", true);
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureOverrideValueInDegFx100", temperature);
   };

   const feelerArmIsReadyToEnterHarvest = async () => {
      await rockhopper.write("Erd_FeelerArmIsReadyToEnterHarvest", true);
   };

   const feelerArmIsNotReadyToEnterHarvest = async () => {
      await rockhopper.write("Erd_FeelerArmIsReadyToEnterHarvest", false);
   };

   const theFillTubeHeaterVoteAndCareShouldBe = async (percentageDutyCycle, care) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerFillTubeHeater_IceMakerVote");
      expect(vote.percentageDutyCycle).toEqual(percentageDutyCycle);
      expect(vote.care).toEqual(care);
   };

   const theMoldHeaterControlRequestShouldBeForHarvest = async () => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerMoldHeaterControlRequest");
      expect(vote.enable).toEqual(true);
      expect(vote.skipInitialOnTime).toEqual(false);
      expect(vote.onTemperatureInDegFx100, heaterOnTemperatureInDegFx100);
      expect(vote.offTemperatureInDegFx100, heaterOffTemperatureInDegFx100);
   };

   const theMoldHeaterControlRequestShouldBeDisabled = async () => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerMoldHeaterControlRequest");
      expect(vote.enable).toEqual(false);
   };

   const theMoldThermistorIsValid = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue", true);
   };

   const theMoldThermistorIsInvalid = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideRequest", true);
      await rockhopper.write("Erd_AluminumMoldIceMakerMoldThermistor_IsValidOverrideValue", false);
   };

   const skipFillRequestIsSet = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerSkipFillRequest", true);
   };

   const skipFillRequestIsClear = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerSkipFillRequest", false);
   };

   const theSkipFillRequestShouldBeClear = async () => {
      const request = await rockhopper.read("Erd_AluminumMoldIceMakerSkipFillRequest");
      expect(request).toEqual(false);
   };

   const theRakeCompletesAFullRevolution = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerRakeCompletedRevolution", true);
   };

   const theRakeDidNotCompleteAFullRevolution = async () => {
      await rockhopper.write("Erd_AluminumMoldIceMakerRakeCompletedRevolution", false);
   };

   const theRakeControllerRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldIceMakerRakeControlRequest");
      expect(actual).toEqual(expected);
   };

   const theWaterFillMonitoringRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldIceMakerWaterFillMonitoringRequest");
      expect(actual).toEqual(expected);
   };

   const providedTheAluminumMoldIceMakerTestRequestIsSetTo = async (request) => {
      await rockhopper.write("Erd_AluminumMoldIceMakerTestRequest", request);
   };

   const theAluminumMoldIceMakerTestRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_AluminumMoldIceMakerTestRequest");
      expect(actual).toEqual(expected);
   };

   const stopFillSignalIsSent = async () => {
      let signal = await rockhopper.read("Erd_AluminumMoldIceMakerStopFillSignal");
      signal++;
      await rockhopper.write("Erd_AluminumMoldIceMakerStopFillSignal", signal);
   };

   it("should be in thermistor fault state if thermistor is invalid after init", async () => {
      await theMoldThermistorIsValid();
      await sabbathModeIsDisabled();
      await iceMakerIsEnabled();
      await theMoldThermistorIsInvalid();

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });
   it("should turn off all loads if thermistor is invalid", async () => {
      await theMoldThermistorIsValid();
      await theMoldThermistorIsInvalid();
      await sabbathModeIsDisabled();
      await iceMakerIsEnabled();

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
      await theIceMakerHeaterShouldBeVoted(relayState.off)
      await theIceMakerMotorShouldBeVoted(motorState.off);
      await theIceMakerWaterValveShouldBeVoted(waterValveState.off);
      await theIsolationWaterValveShouldBeVoted(waterValveState.off);
   });
   it("should transition from fill state to thermistor fault if thermistor is invalid and stop fill signal is sent", async () => {
      await theMoldThermistorIsValid();
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);

      await stopFillSignalIsSent();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should enter freeze state after initialization when Sabbath is disabled and ice maker is enabled", async () => {
      await theMoldThermistorIsValid();
      await sabbathModeIsDisabled();
      await iceMakerIsEnabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is enabled and ice maker is enabled", async () => {
      await theMoldThermistorIsValid();
      await sabbathModeIsEnabled();
      await iceMakerIsEnabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is enabled and ice maker is disabled", async () => {
      await theMoldThermistorIsValid();
      await sabbathModeIsEnabled();
      await iceMakerIsDisabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is disabled and ice maker is disabled", async () => {
      await theMoldThermistorIsValid();
      await sabbathModeIsDisabled();
      await iceMakerIsDisabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should turn off loads on entry to idle freeze state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.idleFreeze);

      await theWaterValveShouldBeVoted(waterValveState.off);
      await theIceMakerHeaterShouldBeVoted(heaterState.off);
      await theIceMakerMotorShouldBeVoted(motorState.off);
   });

   it("should turn off loads on entry to freeze state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await theIceMakerWaterValveShouldBeVoted(waterValveState.off);
      await theIceMakerHeaterShouldBeVoted(relayState.off);
      await theIceMakerMotorShouldBeVoted(motorState.off);
   });

   it("should request to monitor feeler arm on entry to freeze state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await harvestCountCalculationRequestShouldBe(constants.set);
   });

   it("should request to calculate harvest count on entry to freeze state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await feelerArmMonitoringRequestShouldBe(constants.set);
   });

   it("should transition from freeze to harvest when harvest count is ready to harvest and harvest conditions are met", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should not transition from freeze to harvest when feeler arm is ready to harvest and harvest count is not ready to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should not transition from freeze to harvest when harvest count is ready to harvest and ice maker temperature is not ready to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsNotReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should not transition from freeze to harvest when harvest count is ready to harvest and feeler arm is not ready to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should transition from freeze to harvest when ice maker temperature is ready to harvest and harvest conditions are met", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsReadyToHarvest();
      await iceMakerTemperatureIsNotReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await iceMakerTemperatureIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should transition from freeze to harvest when feeler arm is ready to enter harvest and harvest conditions are met", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should request to stop calculating harvest count when transitioning from freeze to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInFreezeState().andWaitingForHarvestCountToBeReadyForHarvest();
      await harvestCountCalculationRequestShouldBe(constants.set);

      await harvestCountIsReadyToHarvest();
      await harvestCountCalculationRequestShouldBe(constants.clear);
   });

   it("should request to stop monitoring feeler arm when transitioning from freeze to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInFreezeState().andWaitingForFeelerArmToBeReadyToEnterHarvest();
      await feelerArmMonitoringRequestShouldBe(constants.set);

      await feelerArmIsReadyToEnterHarvest();
      await feelerArmMonitoringRequestShouldBe(constants.clear);
   });

   it("should transition from freeze to idle freeze when Sabbath is enabled", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await sabbathModeIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from harvest to idle freeze when Sabbath is enabled", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await sabbathModeIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from idle freeze to freeze when Sabbath is disabled", async () => {
      await providedTheAluminumIceMakerIsInIdleFreeze().andSabbathModeIsEnabled();

      await sabbathModeIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should transition from freeze to idle freeze when ice maker is disabled", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await iceMakerIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from harvest to idle freeze when ice maker is disabled", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await iceMakerIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from idle freeze to freeze when ice maker is enabled", async () => {
      await (await providedTheAluminumIceMakerIsInIdleFreeze()).andIceMakerIsDisabled();

      await iceMakerIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should vote to turn on ice maker water valve when entering fill state", async () => {
      await theMoldThermistorIsValid();
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();
      await theIceMakerWaterValveShouldBeVoted(waterValveState.off);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
      await theIceMakerWaterValveShouldBeVoted(waterValveState.on);
   });

   it("should vote to turn on isolation water valve when entering fill state", async () => {
      await theMoldThermistorIsValid();
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();
      await theIsolationWaterValveShouldBeVotedDontCare();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
      await theIsolationWaterValveShouldBeVoted(waterValveState.on);
   });

   it("should request to start water fill monitoring when entering fill state", async () => {
      await theMoldThermistorIsValid();
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();
      await theWaterFillMonitoringRequestShouldBe(constants.clear);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
      await theWaterFillMonitoringRequestShouldBe(constants.set);
   });

   it("should transition from fill to freeze state after timed ice maker fill when no flow meter is present", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should vote to turn off ice maker valve when exiting fill state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      await theIceMakerWaterValveShouldBeVoted(waterValveState.off);
   });

   it("should vote to turn off isolation valve when exiting fill state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      await theIsolationWaterValveShouldBeVoted(waterValveState.off);
   });

   it("should clear water fill monitoring request when exiting fill state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      await theWaterFillMonitoringRequestShouldBe(constants.clear);
   });

   it("should request water fill monitoring when entering fill state again", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      await theWaterFillMonitoringRequestShouldBe(constants.clear);

      await feelerArmIsReadyToEnterHarvest();
      await harvestCountIsReadyToHarvest();

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
      await theWaterFillMonitoringRequestShouldBe(constants.clear);

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
      await theWaterFillMonitoringRequestShouldBe(constants.set);
   });

   // Parametric
   // fill tube heater on time = max harvest time
   it("should vote for fill tube heater on entry to Harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.care);
   });

   it("should vote to dont care for fill tube heater after fill tube heater on time", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.care);

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);

      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.dontCare);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   it("should set mold control request on entry to harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await theMoldHeaterControlRequestShouldBeForHarvest();
   });

   it("should transition from Harvest to Harvest Fix when max harvest time reached", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await theRakeDidNotCompleteAFullRevolution();

      await after(maximumHarvestTimeInMinutes - 1).inMin();
      await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   it("should vote to dont care for fill tube heater after max harvest time", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await after(maximumHarvestTimeInMinutes - 1).inMin();
      await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.dontCare);
   });

   it("should transition from Idle Freeze to Thermistor Fault when mold thermistor is invalid", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.idleFreeze);
      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);

      await sabbathModeIsDisabled();
      await iceMakerIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should transition from Freeze to Thermistor Fault State when mold thermistor is invalid", async () => {
      await theMoldThermistorIsValid();
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should transition from Fill to Thermistor Fault State when mold thermistor is invalid and stop fill signal is sent", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);

      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);

      await stopFillSignalIsSent();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should transition from Harvest to Thermistor Fault State when mold thermistor is invalid", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should transition from Harvest Fix to Thermistor Fault State when mold thermistor is invalid", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvestFix);

      await theMoldThermistorIsInvalid();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should transition from Harvest to Freeze state when fill tube heater timer expired and rake has completed full revolution and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should transition from Harvest to Fill state when fill tube heater timer expired and rake has completed full revolution and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   it("should clear rake controller request when rake completes full revolution during harvest", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await after(initialMinimumHeaterOnTimeInSeconds).inSec();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theRakeControllerRequestShouldBe(true);

      await theRakeCompletesAFullRevolution();
      await theRakeControllerRequestShouldBe(false);
   });

   it("should transition from Harvest to Harvest Fix state when rake has not completed full revolution before fill tube heater timer has expired and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   it("should transition from Harvest to Harvest Fix state when rake has not completed full revolution before fill tube heater timer has expired and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   it("should disable mold heater control request when transitioning from Harvest to Harvest Fix", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
      await theMoldHeaterControlRequestShouldBeForHarvest();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
      await theMoldHeaterControlRequestShouldBeDisabled();
   });

   it("should clear skip fill flag when transitioning from Harvest to Harvest Fix", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
      await theSkipFillRequestShouldBeClear();
   });

   it("should clear rake controller request when transitioning from Harvest to Harvest Fix and the rake has not completed a full revolution", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await after(initialMinimumHeaterOnTimeInSeconds).inSec();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theRakeControllerRequestShouldBe(true);

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - initialMinimumHeaterOnTimeInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
      await theRakeControllerRequestShouldBe(false);
   });

   it("should go to thermistor fault if thermistors become invalid during fill", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);
      await theMoldThermistorIsInvalid();
      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.thermistorFault);
   });

   it("should go to harvest if rake is not home and thermistor is valid", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);
      await theMoldThermistorIsValid();
      await theRakePositionIsNotHome();
      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should go to freeze if rake is home and thermistor is valid", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.fill);
      await theMoldThermistorIsValid();
      await theRakePositionIsHome();
      await delay(timedIceMakerFillInSecondsx10 * msPerSec / 10);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should reset ice maker test request when fill is requested", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await providedTheAluminumMoldIceMakerTestRequestIsSetTo(iceMakerTestRequest.fill);

      await theAluminumMoldIceMakerTestRequestShouldBe(iceMakerTestRequest.none);
   });

   it("should transition from harvest to fill state when external fill request is sent", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await providedTheAluminumMoldIceMakerTestRequestIsSetTo(iceMakerTestRequest.fill);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   it("should transition from freeze to fill state when external fill request is sent", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await providedTheAluminumMoldIceMakerTestRequestIsSetTo(iceMakerTestRequest.fill);

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   // Parametric
   // fill tube heater on time < max harvest time
   // freezeThawFillTubeHeaterOnTimeInSeconds < maximumHarvestTimeInMinutes * 60
   xit("should vote dont care for fill tube heater after fill tube heater on time", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.care);

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theFillTubeHeaterVoteAndCareShouldBe(freezeThawFillTubeHeaterDutyCyclePercentage, voteCare.dontCare);
   });

   xit("should transition from Harvest to Freeze state when fill tube heater timer expired and rake has completed full revolution and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   xit("should transition from Harvest to Fill state when fill tube heater timer expired and rake has completed full revolution and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theRakeCompletesAFullRevolution();

      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   xit("should transition from Harvest to Freeze state when rake completes full revolution and fill tube heater timer has expired and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   xit("should transition from Harvest to Fill state when rake completes full revolution and fill tube heater timer has expired and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   xit("should transition from Harvest to Harvest Fix state when rake has not completed full revolution before max harvest time has expired and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(maximumHarvestTimeInMinutes - 1).inMin();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   xit("should transition from Harvest to Harvest Fix state when rake has not completed full revolution before max harvest time has expired and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(maximumHarvestTimeInMinutes - 1).inMin();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
   });

   xit("should clear rake controller request when transitioning from Harvest to Harvest Fix and the rake has not completed a full revolution", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await after(initialMinimumHeaterOnTimeInSeconds).inSec();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theRakeControllerRequestShouldBe(true);

      await after(maximumHarvestTimeInMinutes - initialMinimumHeaterOnTimeInSeconds).inMin();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await delay(aLittleBitMoreThanAMinuteAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvestFix);
      await theRakeControllerRequestShouldBe(false);
   });

   // Parametric
   // fill tube heater zero duty cycle
   // freezeThawFillTubeHeaterDutyCyclePercentage = 0
   xit("should transition from Harvest to Freeze state when rake completes full revolution and fill tube heater timer has expired and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   xit("should transition from Harvest to Fill state when rake completes full revolution and fill tube heater timer has expired and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await after(freezeThawFillTubeHeaterOnTimeInSeconds - aLittleBitAccelerationDelayInSeconds).inSec();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });

   // Parametric
   // fill tube heater on time zero
   // freezeThawFillTubeHeaterOnTimeInSeconds = 0
   xit("should transition from Harvest to Freeze state when rake completes full revolution and fill tube heater timer has expired and skip fill request is set", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsSet();

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   xit("should transition from Harvest to Fill state when rake completes full revolution and fill tube heater timer has expired and skip fill request is clear", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);
      await skipFillRequestIsClear();

      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);

      await theRakeCompletesAFullRevolution();
      await delay(aLittleBitAccelerationDelayInSeconds * constants.msPerSec);
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.fill);
   });
});
