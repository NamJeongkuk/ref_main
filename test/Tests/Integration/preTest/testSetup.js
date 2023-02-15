"use strict";

const geaNode = require("gea-communication").DualNode();
const Client = require("erd-core").Client;
const FeatureManager = require("./FeatureManager");
const ErdInfoSphere = require("./ErdInfoSphere");
const { msPerMin } = require("../support/constants");

geaNode.bind();

const infoSphere = ErdInfoSphere();

const address = 0xe4;
const client = Client({
   geaNode,
   address,
   infoSphere,
});

const rockhopper = {
   write: async (erdName, data) => {
      await client.write({ host, erd: erdName, data });
   },
   read: async (erdName) => {
      return await client.read({ host, erd: erdName });
   },
};

global["rockhopper"] = rockhopper;
global["geaSource"] = address;

const featureManager = FeatureManager(geaNode);
jasmine.getEnv().addReporter(featureManager);

jasmine.DEFAULT_TIMEOUT_INTERVAL = 20 * msPerMin;
