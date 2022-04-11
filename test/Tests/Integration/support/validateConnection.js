const delay = require('javascript-common').util.delay;
const GeaMessages = require('javascript-common').GeaMessages;
const GeaStream = require('javascript-common').GeaStream;

const toHex = (x) => '0x' + x.toString(16);

module.exports = async (geaNode) => {
   const source = geaSource;
   const destination = hostDestination;

   const geaMessages = GeaMessages(geaNode);
   const geaStream = GeaStream(geaNode, destination, source);

   await geaMessages.bootLoader.reset(source, destination, 0);
   await delay(2000);

   await geaMessages.application.getVersion(source, destination, geaStream, 1000).catch(() => {
      if (!geaNode.connected()) {
         console.log(`Could not communicate with green / pink bean, ensure that one is connected and that ApplCore is not running.`);
      } else {
         console.log(`Board ${toHex(destination)} not detected, ensure that it is connected and can respond to a version query (0x01)`);
      }
      process.exit();
   });
};
