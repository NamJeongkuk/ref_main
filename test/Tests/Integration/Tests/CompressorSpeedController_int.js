"use strict";

const delay = require("javascript-common").util.delay;
const { msPerSec } = require("../support/constants");
const constants = require("../support/constants");

const after = (time) => ({
   inMsec: async () => {
      await delay(time);
   },
   inSec: async () => {
      await delay(time * msPerSec);
   }
});

xdescribe("CompressorSpeedController,", () => {
   const compressorStartupSpeedInHz = 21;
   const someOtherCompressorSpeedInHz = 26;

   beforeEach(async () => {
      await rx130.write("Erd_Reset", 1);
      await delay(constants.oneSecondInMsec * 5);
   });
})
