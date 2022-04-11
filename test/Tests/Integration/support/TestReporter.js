module.exports = (geaNode, infoSphere) => {
   return {
      jasmineStarted: async (suiteInfo) => { },
      suiteStarted: async (result) => { },
      specStarted: async (result) => { },
      specDone: async (result) => { },
      suiteDone: async (result) => { },
      jasmineDone: async (result) => { }
   };
};
