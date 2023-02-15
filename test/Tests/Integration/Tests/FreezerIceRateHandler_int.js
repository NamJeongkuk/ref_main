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

describe("FreezerIceRateHandler", () => {
   const freezerIceRateTimeInMinutes = 2;
   const integrationTestDelayInMsec = 1000;

   const iceRateActiveErdShouldBe = async (expected) => {
      const actual = await rockhopper.read("Erd_Freezer_IceRateIsActive");
      expect(actual).toEqual(expected);
   }

   const providedIceRateTriggerSignalIsSent = async () => {
      let currentSignal = await rockhopper.read("Erd_FreezerIceRateTriggerSignal");
      currentSignal = currentSignal + 1;
      await rockhopper.write("Erd_FreezerIceRateTriggerSignal", currentSignal);
   }

   beforeEach(async () => {
      await rockhopper.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });

   it('ice rate active erd should be false before trigger and true after', async () => {
      await iceRateActiveErdShouldBe(false);
      await providedIceRateTriggerSignalIsSent();
      await iceRateActiveErdShouldBe(true);
   });

   it('ice rate active erd should be true for ice rate time in minutes', async () => {
      await iceRateActiveErdShouldBe(false);

      await providedIceRateTriggerSignalIsSent();
      await iceRateActiveErdShouldBe(true);

      await after(freezerIceRateTimeInMinutes).inMin();
      await delay(integrationTestDelayInMsec);
      await iceRateActiveErdShouldBe(false);
   });

   it('ice rate active erd should be true when signal is reactivated', async () => {
      await providedIceRateTriggerSignalIsSent();
      await iceRateActiveErdShouldBe(true);

      await after(freezerIceRateTimeInMinutes).inMin();
      await delay(integrationTestDelayInMsec);
      await iceRateActiveErdShouldBe(false);

      await providedIceRateTriggerSignalIsSent();
      await iceRateActiveErdShouldBe(true);
   });
});
