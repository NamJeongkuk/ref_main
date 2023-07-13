'use strict';

const delay = require('javascript-common').util.delay;

const aFewMilliseconds = 10;

module.exports = {
   advance: async (msec) => {
      await rockhopper.write('Erd_TimeAcceleration_Ticks', msec);

      const signal = await rockhopper.read(`Erd_TimeAcceleration_CompleteSignal`);
      await rockhopper.write('Erd_TimeAcceleration_Enable', true);
      do {
         await delay(aFewMilliseconds);
      } while (signal === await rockhopper.read('Erd_TimeAcceleration_CompleteSignal'));
   }
};
