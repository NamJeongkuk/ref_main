'use strict';

const { resolve } = require('path');
const { inspect } = require('util');
const constructIntegrationTestTools = require('integration-testing-tools');

const jsonDirectory = resolve(__dirname, '../../../build/rockhopper/doc');

jasmine.getEnv().addReporter({
  specStarted: (result) => {
    console.log(`\n---\n`);
  },
  specDone: (result) => {
    if (result.status === 'failed') {
      console.error(`\n${inspect(result.failedExpectations)}`);
    }
  },
  suiteStarted: (result) => {
    console.log(`------------------------`);
    console.log(`Describe '${result.fullName}'`);
    console.log(`------------------------`);
  },
  jasmineDone: () => console.log(`------------------------`),
});

module.exports = (async () => {
  const integrationTestTools = await constructIntegrationTestTools([jsonDirectory]);

  integrationTestTools.configureDestination('mb', '_board');
  integrationTestTools.addResetValidationToDestination(_board, 'Erd_ResetCount');

  await integrationTestTools.setRequiredAddresses([0xc0]);

  return integrationTestTools;
})();
