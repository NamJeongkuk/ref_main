const GeaMessages = require('javascript-common').GeaMessages;
const GeaStream = require('javascript-common').GeaStream;
const ConnectionTools = require('../support/ConnectionTools');

module.exports = async (geaNode) => {
   const source = geaSource;
   const geaMessages = GeaMessages(geaNode);
   const primaryHost = 'host';
   const secondaryHost = 'host_secondary';

   const queryBoard = async (destination) => {
      geaStream = GeaStream(geaNode, destination, source);
      return await geaMessages.application.getVersion(source, destination, geaStream, 1000);
   };

   const errorLog = () => {
      if (!geaNode.connected()) {
         console.log(`Could not communicate with green / pink bean, ensure that one is connected and that ApplCore is not running.`);
      } else {
         console.log(`Board not detected, ensure that it is connected and can respond to a version query (0x01)`);
      }
   };

   const setHostDestination = (hostType, hostName, hostAddress) => {
      global[hostType] = hostName;
      let hostDestination = (hostType == primaryHost) ? 'hostDestination' : 'hostDestinationSecondary';
      global[hostDestination] = hostAddress;
   };

   const isHostSet = (host) => {
      return typeof (global[host]) !== 'undefined' && global[host] !== null;
   };

   const checkNeedToQuery = (hostType) => {
      return !((isHostSet(primaryHost) && isHostSet(secondaryHost)) ||
         (hostType == 'primary' && isHostSet(primaryHost)));
   };

   const configureHost = (host, address, type) => {
      if (type == 'primary') {
         setHostDestination(primaryHost, host, address);
      } else if (host == 'secondary') {
         setHostDestination(secondaryHost, host, address);
      }
   };

   const processHostList = async (list) => {
      for (const item of list) {
         if (checkNeedToQuery(item.type)) {
            await queryBoard(item.address).then(() => {
               configureHost(item.host, item.address, item.type);
            }).catch(() => { });
         }
      }
   };

   const hostList = [
      { host: 'rx130', address: 0xC0, type: 'primary' },
   ];

   await ConnectionTools.resetBootloader(geaNode);
   await processHostList(hostList);

   if (!isHostSet(primaryHost)) {
      errorLog();
      process.exit();
   }
};
