const ConnectionTools = require('../support/ConnectionTools');
const establishConnectionToDestinationBoard = require('../support/establishConnectionToDestinationBoard');

module.exports = (geaNode) => {
   let needToValidateConnection = true;
   let needToResetControl = true;

   global['test'] = (title, feature, f) => {
      it(title, callIfFeatureIsEnabled(feature, f));
   };

   global['xtest'] = (title, _, f) => {
      xit(title, f);
   };

   global['ftest'] = (title, feature, f) => {
      fit(title, callIfFeatureIsEnabled(feature, f));
   };

   return {
      jasmineStarted: async (suiteInfo) => {
         await establishConnectionToDestinationBoard(geaNode);
      },
      suiteStarted: async (result) => {
         needToValidateConnection = true;
         needToResetControl = true;
      }
   };
};
