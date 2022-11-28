"use strict";

const delay = require("javascript-common").util.delay;
const constants = require("../support/constants");

fdescribe("AluminumMoldIceMaker,", () => {
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

   const heaterState = {
      off: "HeaterState_Off",
      on: "HeaterState_On"
   };

   const motorState = {
      off: "MotorState_Off",
      on: "MotorState_On"
   };

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
      await rockhopper.write("Erd_IceMakerEnabledByUser", true);
   };

   const iceMakerIsDisabled = async () => {
      await rockhopper.write("Erd_IceMakerEnabledByUser", false);
   };

   const resetBoard = async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   };

   const providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState = async(state) => {
      if(state == hsmState.freeze) {
         await sabbathModeIsDisabled();
         await iceMakerIsEnabled();
         await resetBoard();
      }
      else if(state == hsmState.harvest) {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsNotReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsReadyToEnterHarvest();

         await harvestCountIsReadyToHarvest();
      }
      await theAluminumMoldIceMakerHsmStateShouldBe(state);
   };

   const providedTheAluminumMoldIceMakerIsInFreezeState = () => ({
      andWaitingForHarvestCountToBeReadyForHarvest: async() => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsNotReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsReadyToEnterHarvest();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      },
      andWaitingForFeelerArmToBeReadyToEnterHarvest: async() => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
         await harvestCountIsReadyToHarvest();
         await iceMakerTemperatureIsReadyToHarvest();
         await feelerArmIsNotReadyToEnterHarvest();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
      }
   });

   const providedTheAluminumIceMakerIsInIdleFreeze = () => ({
      andSabbathModeIsEnabled: async() => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

         await sabbathModeIsEnabled();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
      },
      andIceMakerIsDisabled: async() => {
         await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

         await iceMakerIsDisabled();
         await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
      }
   });

   const theWaterValveShouldBeVoted = async(state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerWaterValve_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(true);
   };

   const theIceMakerHeaterShouldBeVoted = async(state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerHeater_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(true);
   };

   const theIceMakerMotorShouldBeVoted = async(state) => {
      const vote = await rockhopper.read("Erd_AluminumMoldIceMakerMotor_IceMakerVote");
      expect(vote.state).toEqual(state);
      expect(vote.care).toEqual(true);
   };

   const harvestCountCalculationRequestShouldBe = async(state) => {
      const actual = await rockhopper.read("Erd_HarvestCountCalculationRequest");
      expect(actual).toEqual(state);
   };

   const feelerArmMonitoringRequestShouldBe = async(state) => {
      const actual = await rockhopper.read("Erd_FeelerArmMonitoringRequest");
      expect(actual).toEqual(state);
   };

   const harvestCountIsNotReadyToHarvest = async() => {
      await rockhopper.write("Erd_HarvestCountIsReadyToHarvest", false);
   };

   const harvestCountIsReadyToHarvest = async() => {
      await rockhopper.write("Erd_HarvestCountIsReadyToHarvest", true);
   };

   const iceMakerTemperatureIsReadyToHarvest = async() => {
      let temperature = 1900 - 1; // max harvest temperature - 1
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100", temperature);
   };

   const iceMakerTemperatureIsNotReadyToHarvest = async() => {
      let temperature = 1900; // max harvest temperature
      await rockhopper.write("Erd_AluminumMoldIceMaker_FilteredTemperatureInDegFx100", temperature);
   };

   const feelerArmIsReadyToEnterHarvest = async() => {
      await rockhopper.write("Erd_FeelerArmIsReadyToEnterHarvest", true);
   };

   const feelerArmIsNotReadyToEnterHarvest = async() => {
      await rockhopper.write("Erd_FeelerArmIsReadyToEnterHarvest", false);
   };

   it("should enter freeze state after initialization when Sabbath is disabled and ice maker is enabled", async () => {
      await sabbathModeIsDisabled();
      await iceMakerIsEnabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is enabled and ice maker is enabled", async () => {
      await sabbathModeIsEnabled();
      await iceMakerIsEnabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is disabled and ice maker is disabled", async () => {
      await sabbathModeIsEnabled();
      await iceMakerIsDisabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should enter idle freeze state after initialization when Sabbath is disabled and ice maker is disabled", async () => {
      await sabbathModeIsDisabled();
      await iceMakerIsDisabled();
      await resetBoard();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should turn off loads on entry to freeze state", async () => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await theWaterValveShouldBeVoted(waterValveState.off);
      await theIceMakerHeaterShouldBeVoted(heaterState.off);
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

   it("should transition from freeze to harvest when harvest count is ready to harvest and harvest conditions are met", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should not transition from freeze to harvest when feeler arm is ready to harvest and harvest count is not ready to harvest", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should not transition from freeze to harvest when harvest count is ready to harvest and ice maker temperature is not ready to harvest", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsNotReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should not transition from freeze to harvest when harvest count is ready to harvest and feeler arm is not ready to harvest", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsNotReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await harvestCountIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should transition from freeze to harvest when ice maker temperature is ready to harvest and harvest conditions are met", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsReadyToHarvest();
      await iceMakerTemperatureIsNotReadyToHarvest();
      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await iceMakerTemperatureIsReadyToHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should transition from freeze to harvest when feeler arm is ready to enter harvest and harvest conditions are met", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);
      await harvestCountIsReadyToHarvest();
      await iceMakerTemperatureIsReadyToHarvest();
      await feelerArmIsNotReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);

      await feelerArmIsReadyToEnterHarvest();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.harvest);
   });

   it("should request to stop calculating harvest count when transitioning from freeze to harvest", async() => {
      await providedTheAluminumMoldIceMakerIsInFreezeState().andWaitingForHarvestCountToBeReadyForHarvest();
      await harvestCountCalculationRequestShouldBe(constants.set);

      await harvestCountIsReadyToHarvest();
      await harvestCountCalculationRequestShouldBe(constants.clear);
   });

   it("should request to stop monitoring feeler arm when transitioning from freeze to harvest", async() => {
      await providedTheAluminumMoldIceMakerIsInFreezeState().andWaitingForFeelerArmToBeReadyToEnterHarvest();
      await feelerArmMonitoringRequestShouldBe(constants.set);

      await feelerArmIsReadyToEnterHarvest();
      await feelerArmMonitoringRequestShouldBe(constants.clear);
   });

   it("should transition from freeze to idle freeze when Sabbath is enabled", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await sabbathModeIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from harvest to idle freeze when Sabbath is enabled", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await sabbathModeIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from idle freeze to freeze when Sabbath is disabled", async() => {
      await providedTheAluminumIceMakerIsInIdleFreeze().andSabbathModeIsEnabled();

      await sabbathModeIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });

   it("should transition from freeze to idle freeze when ice maker is disabled", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.freeze);

      await iceMakerIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from harvest to idle freeze when ice maker is disabled", async() => {
      await providedTheAluminumMoldIceMakerIsInitializedAndGetsIntoState(hsmState.harvest);

      await iceMakerIsDisabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.idleFreeze);
   });

   it("should transition from idle freeze to freeze when ice maker is enabled", async () => {
      await (await providedTheAluminumIceMakerIsInIdleFreeze()).andIceMakerIsDisabled();

      await iceMakerIsEnabled();
      await theAluminumMoldIceMakerHsmStateShouldBe(hsmState.freeze);
   });
});
