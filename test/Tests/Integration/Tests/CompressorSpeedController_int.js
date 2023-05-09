"use strict";

const delay = require("javascript-common").util.delay;
const After = require('../support/After.js');
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

const after = (time) => ({
   inMsec: async () => {
      await After(time);
   },
   inSec: async () => {
      await After(time * msPerSec);
   },
   inMin: async () => {
      await After(time * msPerMin);
   }
});

describe("CompressorSpeedController,", () => {

   const compressorState = {
      off: "CompressorState_Off",
      startup: "CompressorState_Startup",
      on: "CompressorState_On",
      offAndReadyToChange: "CompressorState_OffAndReadyToChange",
      onAndReadyToChange: "CompressorState_OnAndReadyToChange",
      minimumOnTime: "CompressorState_MinimumOnTime",
      minimumOffTime: "CompressorState_MinimumOffTime",
      remainOffAfterValveMove: "CompressorState_RemainOffAfterValveMove",
      sabbathDelay: "CompressorState_SabbathDelay",
      variableSpeedMinimumRunTime: "CompressorState_VariableSpeedMinimumRunTime",
   };

   const compressorSpeed = {
      off: "CompressorSpeed_Off",
      superLow: "CompressorSpeed_SuperLow",
      low: "CompressorSpeed_Low",
      medium: "CompressorSpeed_Medium",
      high: "CompressorSpeed_High",
      superHigh: "CompressorSpeed_SuperHigh",
      startup: "CompressorSpeed_Startup"
   };

   const valvePosition = {
      a: "ValvePosition_A",
      b: "ValvePosition_B",
      c: "ValvePosition_C",
      d: "ValvePosition_D",
      e: "ValvePosition_E",
      f: "ValvePosition_F",
      home: "ValvePosition_Home"
   };

   const sabbathTimeInSeconds = 20;
   const minimumOffTimeInMinutes = 10;
   const minimumOnTimeInMinutes = 15;
   const startupTimeInSeconds = 100;
   const variableSpeedMinimumRunTimeInMinutes = 2;
   const remainOffAfterValveMoveTimeInMinutesLowAmbient = 3;
   const remainOffAfterValveMoveTimeInMinutes = 2;

   const someTemperatureBelowTriggerAmbientTemperature = 299;
   const someTemperatureAboveTriggerAmbientTemperature = 301;

   const providedTheFactorySpeedCompressorVoteSpeedIs = async (requestedSpeed) => {
      let requestedVote = { speed: requestedSpeed, care: "Vote_Care" };
      await rockhopper.write("Erd_CompressorSpeed_FactoryVote", requestedVote);
   };

   const providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState = async (requestedCompressorState, requestedSpeed) => {
      if (requestedCompressorState === compressorState.offAndReadyToChange) {
         await theMinimumTimesAreDisabled();
         await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
         await theMinimumTimesAreEnabled();
      }

      else if (requestedCompressorState === compressorState.sabbathDelay) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange, compressorSpeed.off);
         await providedTheFactorySpeedCompressorVoteSpeedIs(requestedSpeed);
      }

      else if (requestedCompressorState === compressorState.startup) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, requestedSpeed);
         await after(sabbathTimeInSeconds).inSec();
      }

      else if (requestedCompressorState === compressorState.minimumOnTime) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, requestedSpeed);
         await after(startupTimeInSeconds).inSec();
         await delay(500);
      }

      else if (requestedCompressorState === compressorState.onAndReadyToChange) {
         await theMinimumTimesAreDisabled();
         await providedTheFactorySpeedCompressorVoteSpeedIs(requestedSpeed);
         await theMinimumTimesAreEnabled();
      }

      else if (requestedCompressorState === compressorState.minimumOffTime) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, requestedSpeed);
         await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      }

      else if (requestedCompressorState === compressorState.variableSpeedMinimumRunTime) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, requestedSpeed);
         await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);
      }

      else if (requestedCompressorState === compressorState.remainOffAfterValveMove) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange, compressorSpeed.off);
         await theValvePositionFactoryVoteIs(valvePosition.b);
      }

      await theCompressorStateShouldBe(requestedCompressorState);
   };

   const theValvePositionFactoryVoteIs = async (requestedPosition) => {
      let requested = { position: requestedPosition, care: "Vote_Care" };
      await rockhopper.write("Erd_ValvePosition_FactoryVote", requested);
   };

   const providedThatTheAmbientTemperatureOverrideIsActive = async () => {
      await rockhopper.write("Erd_Ambient_FilteredInternalTemperatureOverrideRequest", true);
   };

   const theFilteredAmbientTemperatureOverrideIs = async (requestedTemperature) => {
      await providedThatTheAmbientTemperatureOverrideIsActive();
      await rockhopper.write("Erd_Ambient_FilteredInternalTemperatureOverrideValueInDegFx100", requestedTemperature);
   };

   const theValvePositionResolvedVoteShouldBe = async (expected) => {
      let actual = await rockhopper.read("Erd_ValvePosition_ResolvedVote");
      expect(actual.position).toEqual(expected);
   };

   const theCompressorSpeedResolvedVoteShouldBe = async (expected) => {
      let actual = await rockhopper.read("Erd_CompressorSpeed_ResolvedVote");
      expect(actual.speed).toEqual(expected);
   };

   const theCompressorStateShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CompressorState");
      expect(actual).toEqual(expected);
   };

   const theCompressorSpeedRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CompressorControllerSpeedRequest");
      expect(actual).toEqual(expected);
   };

   const theMinimumTimesAreDisabled = async () => {
      const requested = true;
      await rockhopper.write("Erd_DisableMinimumCompressorTimes", requested);
   };

   const theMinimumTimesAreEnabled = async () => {
      const requested = false;
      await rockhopper.write("Erd_DisableMinimumCompressorTimes", requested);
   };

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      await delay(constants.oneSecondInMsec * 5);
   });

   it("should transition to sabbath delay state when compressor speed vote changes to nonzero value after being zero", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange, compressorSpeed.off);

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await theCompressorStateShouldBe(compressorState.sabbathDelay);
   });

   it("should request to startup speed after transitioning to startup state", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds - 1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);

      await after(1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.startup);
   });

   it("should transition to startup state after sabbath delay time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.startup);
   });

   it("should request speed of resolved vote when controller was in off and ready to change after transitioning to minimum on time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds).inSec();
      await delay(500);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to minimum on state after sabbath and startup timers complete with nonzero resolved vote", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds).inSec();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.minimumOnTime);
   });

   it("should request speed of resolved vote when controller was in off and ready to change even if resolved vote changes during sabbath time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds - 1).inSec();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);

      await after(startupTimeInSeconds).inSec();
      await delay(500);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });


   it("should request speed of resolved vote when controller was in on and ready to change even if resolved vote changes during startup time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds - 1).inSec();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);

      await after(1).inSec();
      delay(500);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to on and ready to change from minimum on time after minimum on timer expires", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOnTime, compressorSpeed.low);

      await after(minimumOnTimeInMinutes - 1).inMin();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);

      await after(1).inMin();
      await theCompressorStateShouldBe(compressorState.onAndReadyToChange);
   });

   it("should not request new compressor speed in transition from minimum on time to on and ready to change if speed hasnt changed", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOnTime, compressorSpeed.low);

      await after(minimumOnTimeInMinutes - 1).inMin();
      await delay(500);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);

      await after(1).inMin();
      await delay(500);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition from on and ready to change to minimum off time when resolved speed vote becomes off", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, compressorSpeed.low);

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      await theCompressorStateShouldBe(compressorState.minimumOffTime);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);
   });

   it("should cache resolved voted speed in on and ready to change state when a nonzero speed change occurs and request the cached speed in variable speed minimum run time state when on and ready to change transitions to variable speed minimum run time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, compressorSpeed.low);

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);
      await theCompressorStateShouldBe(compressorState.variableSpeedMinimumRunTime);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.medium);
   });

   it("should transition from variable speed minimum run time to on and ready to change when the minimum run timer completes", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, compressorSpeed.low);
      await theValvePositionFactoryVoteIs(compressorSpeed.medium);
      await after(variableSpeedMinimumRunTimeInMinutes - 1).inMin();
      await delay(500);
      await theCompressorStateShouldBe(compressorState.variableSpeedMinimumRunTime);

      await after(1).inMin();
      await theCompressorStateShouldBe(compressorState.onAndReadyToChange);
   });

   it("should not change requested speed while in minimum run time even if resolved compressor speed vote changes to nonzero speed", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.onAndReadyToChange, compressorSpeed.low);
      await theValvePositionFactoryVoteIs(compressorSpeed.medium);
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.high);
      await theCompressorStateShouldBe(compressorState.variableSpeedMinimumRunTime);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.medium)

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.superLow);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.medium)

   });

   it("should transition from minimum off time to off and ready to change after minimum off time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await after(minimumOffTimeInMinutes - 1).inMin();
      await delay(500);
      await theCompressorStateShouldBe(compressorState.minimumOffTime);

      await after(1).inMin();
      await delay(5000);
      await theCompressorStateShouldBe(compressorState.offAndReadyToChange);
   });

   it("should transition from minimum off time to remain off after valve move when the voted valve position changes", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await theValvePositionFactoryVoteIs(valvePosition.b);

      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);
   });

   it("should transition from remain off after valve move to minimum off time after remain off after valve move timer elapses", async () => {
      await theFilteredAmbientTemperatureOverrideIs(someTemperatureAboveTriggerAmbientTemperature);
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await theValvePositionFactoryVoteIs(valvePosition.b);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(remainOffAfterValveMoveTimeInMinutes - 1).inMin();
      await delay(500);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(1).inMin();
      await delay(500);
      await theCompressorStateShouldBe(compressorState.minimumOffTime);
   });

   it("should transition from remain off after valve move to minimum off time after remain off after valve move timer elapses with low ambient temperature", async () => {
      await theFilteredAmbientTemperatureOverrideIs(someTemperatureBelowTriggerAmbientTemperature);
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await theValvePositionFactoryVoteIs(valvePosition.b);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(remainOffAfterValveMoveTimeInMinutesLowAmbient - 1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.minimumOffTime);
   });

   it("should transition to remain off after valve move then to off and ready to change when valve moves late in minimum off time", async () => {
      await theFilteredAmbientTemperatureOverrideIs(someTemperatureAboveTriggerAmbientTemperature);
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await after(minimumOffTimeInMinutes - 1).inMin();
      await delay(1000);
      await theValvePositionFactoryVoteIs(valvePosition.b);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(remainOffAfterValveMoveTimeInMinutes - 1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.offAndReadyToChange);
   });

   it("should transition to remain off after valve move then to off and ready to change when valve moves late in minimum off time with low ambient temperature", async () => {
      await theFilteredAmbientTemperatureOverrideIs(someTemperatureBelowTriggerAmbientTemperature);
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOffTime, compressorSpeed.low);

      await after(minimumOffTimeInMinutes - 1).inMin();
      await delay(1000);
      await theValvePositionFactoryVoteIs(valvePosition.b);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);


      await after(remainOffAfterValveMoveTimeInMinutesLowAmbient - 1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.remainOffAfterValveMove);

      await after(1).inMin();
      await delay(1000);
      await theCompressorStateShouldBe(compressorState.offAndReadyToChange);
   });

   it("should disable compressor minimum off times in factory mode", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      await after(minimumOffTimeInMinutes - 1).inMin();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);
      await theCompressorSpeedResolvedVoteShouldBe(compressorSpeed.medium);
   });

   it("should disable compressor minimum on times in factory mode", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);
      await after(minimumOnTimeInMinutes - 1).inMin();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      await theCompressorSpeedResolvedVoteShouldBe(compressorSpeed.off);
   });
});
