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

   const sabbathTimeInSeconds = 20;
   const startupTimeInSeconds = 100;
   const minimumOnTimeInMinutes = 15;

   const providedTheFactorySpeedCompressorVoteSpeedIs = async (requestedSpeed) => {
      let requestedVote = { speed: requestedSpeed, care: true };
      await rx130.write("Erd_CompressorSpeed_FactoryVote", requestedVote);
   };

   const theResolvedCompressorSpeedVoteIs = async (expectedSpeed) => {
      let expected = { speed: expectedSpeed, care: true };
      const actual = await rx130.read("Erd_CompressorSpeed_ResolvedVote");
      expect(actual).toEqual(expected);
   };

   const providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState = async (requestedCompressorState) => {
      if (requestedCompressorState === compressorState.offAndReadyToChange) {
         await theResolvedCompressorSpeedVoteShouldBe(compressorSpeed.off);
      }

      else if (requestedCompressorState === compressorState.sabbathDelay) {

         await providedTheCompressorSpeedControllerIsInitializedAndGetsIntoState(compressorState.offAndReadyToChange)
      }

      const actual = await rx130.read("Erd_CompressorControllerSpeedRequest");
      expect(actual).toEqual(requestedCompressorState);
   };

   const theCompressorStateShouldBe = async (expected) => {
      const actual = await rx130.read("Erd_CompressorState");
      expect(actual).toEqual(expected);
   };

   const theCompressorSpeedRequestShouldBe = async (expected) => {
      const actual = await rx130.read("Erd_CompressorControllerSpeedRequest");
      expect(actual).toEqual(expected);
   };

   beforeEach(async () => {
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   it("should init into off and ready to change state when voted speed is off", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.off);
      await theCompressorStateShouldBe(compressorState.offAndReadyToChange);
   });

   it("should transition to sabbath delay state when compressor speed vote changes to nonzero value after being zero", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await theCompressorStateShouldBe(compressorState.sabbathDelay);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);
   });

   it("should request to remain off after transitioning to sabbath state", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);

      after(sabbathTimeInSeconds - 1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.off);
   });

   it("should transition to startup state after sabbath delay time", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);

      await after(sabbathTimeInSeconds - 1).inSec();
      await theCompressorStateShouldBe(compressorState.sabbathDelay);

      await after(1).inSec();
      await theCompressorStateShouldBe(compressorState.startup);
   });

   it("should transition to startup speed after sabbath delay time with nonzero resolved speed vote", async () => {

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.startup);
   });

   it("should request speed of resolved vote when controller was in off and ready to change after transitioning to minimum on time", async () => {

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds + startupTimeInSeconds).inSec();

      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to minimum on state after sabbath and startup timers complete with NonZero resolved vote", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds + startupTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);
   });

   it("should request speed of resolved vote when controller was in off and ready to change even if resolved vote changes during sabbath time", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds - 1).inSec();

      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);
      await after(startupTimeInSeconds + 1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });


   it("should request speed of resolved vote when controller was in off and ready to change even if resolved vote changes during startup time", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds + startupTimeInSeconds - 1).inSec();
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.medium);

      await after(1).inSec();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });

   it("should transition to on and ready to change from minimum on time after minimum on timer expires", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds + startupTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);

      await after(minimumOnTimeInMinutes - 1).inMin();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);

      await after(1).inMin();
      await theCompressorStateShouldBe(compressorState.onAndReadyToChange);
   });

   it("should not request compressor speed in transition from minimum on time to on and ready to change", async () => {
      await providedTheFactorySpeedCompressorVoteSpeedIs(compressorSpeed.low);
      await after(sabbathTimeInSeconds + startupTimeInSeconds).inSec();
      await theCompressorStateShouldBe(compressorState.minimumOnTime);
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);

      await after(minimumOnTimeInMinutes).inMin();
      await theCompressorSpeedRequestShouldBe(compressorSpeed.low);
   });
});
