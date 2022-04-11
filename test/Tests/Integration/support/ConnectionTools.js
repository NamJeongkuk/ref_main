const delay = require('javascript-common').util.delay;
const GeaMessages = require('javascript-common').GeaMessages;
const GeaStream = require('javascript-common').GeaStream;
const constants = require('./constants');

const toHex = (x) => '0x' + x.toString(16);

module.exports = {
   validateConnection: async (geaNode) => {
      const source = geaSource;
      const destination = hostDestination;
      const connectionVerificationDelay = constants.msPerSec;

      const geaMessages = GeaMessages(geaNode);
      const geaStream = GeaStream(geaNode, destination, source);

      await geaMessages.application.getVersion(source, destination, geaStream, connectionVerificationDelay).catch(() => {
         console.log("Could not communicate with target, trying again...");
      }).finally(async () => {
         await geaMessages.application.getVersion(source, destination, geaStream, connectionVerificationDelay).catch(() => {
            if (!geaNode.connected()) {
               console.log(`Could not communicate with green / pink bean, ensure that one is connected and that ApplCore is not running.`);
            } else {
               console.log(`Board ${toHex(destination)} not detected, ensure that it is connected and can respond to a version query (0x01)`);
            }
            process.exit();
         });
      });
   },

   resetBootloader: async (geaNode) => {
      const source = geaSource;
      const geaMessages = GeaMessages(geaNode);

      await geaMessages.bootLoader.reset(source, 0xFF, 0);
      await delay(constants.bootLoaderResetDelay);
   }
};
