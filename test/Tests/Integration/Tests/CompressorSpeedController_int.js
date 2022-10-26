"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec, msPerMin } = require("../support/constants");
const constants = require("../support/constants");

const after = (time) => ({
   inMsec: async () => {
      await delay(time);
   },
   inSec: async () => {
      await delay(time * msPerSec);
   },
   inMin: async () => {
      await delay(time * msPerMin);
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

   // Adding +1 on purpose as a buffer
   const sabbathTimeInSeconds = 20 + 1;
   const startupTimeInSeconds = 100 + 1;
   const minimumOnTimeInMinutes = 15;

   const providedTheFactorySpeedCompressorVoteSpeedIs = async (requestedSpeed) => {
      let requestedVote = { speed: requestedSpeed, care: true };
      await rockhopper.write("Erd_CompressorSpeed_FactoryVote", requestedVote);
   };

   const providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState = async (requestedCompressorState, requestedSpeed) => {

      if (requestedCompressorState === compressorState.offAndReadyToChange) {
         expect(requestedSpeed).toEqual(compressorSpeed.off);
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

      }

      else if (requestedCompressorState === compressorState.onAndReadyToChange) {
         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOnTime, requestedSpeed);
         await after(minimumOnTimeInMinutes).inMin();
      }

      await theCompressorStateShouldBe(requestedCompressorState);
   };

   const theCompressorStateShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CompressorState");
      expect(actual).toEqual(expected);
   };

   const theCompressorSpeedRequestShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_CompressorControllerSpeedRequest");
      expect(actual).toEqual(expected);
   };

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   it("should init into off and ready to change state when voted speed is off", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange, compressorSpeed.off);
   });

   it("should transition to sabbath delay state when compressor speed vote changes to nonzero value after being zero", async () => {

      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange, compressorSpeed.off);

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await theCompressorStateShouldBe(compressorState.sabbathDelay);
   });

   it("should request to remain off after transitioning to sabbath state", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      after(sabbathTimeInSeconds - 1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);
   });

   it("should transition to startup state after sabbath delay time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.startup);
   });

   it("should transition to startup speed after sabbath delay time with nonzero resolved speed vote", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.startup);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.startup);
   });

   it("should request speed of resolved vote when controller was in off and ready to change after transitioning to minimum on time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to minimum on state after sabbath and startup timers complete with nonzero resolved vote", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);
   });

   it("should request speed of resolved vote when controller was in off and ready to change even if resolved vote changes during sabbath time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.sabbathDelay, compressorSpeed.low);

      await after(sabbathTimeInSeconds - 1).inSec();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);

      await after(startupTimeInSeconds + 1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });


   it("should request speed of resolved vote when controller was in off and ready to change even if resolved vote changes during startup time", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.startup, compressorSpeed.low);

      await after(startupTimeInSeconds - 1).inSec();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);

      await after(1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to on and ready to change from minimum on time after minimum on timer expires", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOnTime, compressorSpeed.low);

      await after(minimumOnTimeInMinutes - 1).inMin();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);

      await after(1).inMin();
      await theCompressorStateShouldBe(compressorState.onAndReadyToChange);
   });

   it("should not request compressor speed in transition from minimum on time to on and ready to change", async () => {
      await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.minimumOnTime, compressorSpeed.low);

      await after(minimumOnTimeInMinutes - 1).inMin();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);

      await after(1).inMin();
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
});
