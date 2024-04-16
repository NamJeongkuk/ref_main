'use strict';

const board = require('../setup/board');
const Tracker = require('../rockhopper/ErdTracker');
const defrost = require('../rockhopper/defrost');

const delay = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

const print = console.log;

describe('bench', () => {
  beforeEach(async () => {
    // runs before each test
  });

  afterEach(async () => {
    // runs after each test
  });

  beforeAll(async () => {
    // runs once before all tests OR describes in this describe
  });

  afterAll(async () => {
    // runs once after all tests OR describes in this describe
  });

  describe('defrost', () => {
    beforeEach(async () => {
      // only runs for tests in this describe
      await defrost.given.isInState('PrechillPrep');
    });

    xtest('should get into prechill', async () => {
      await board.expect('Erd_DefrostHsmState').toEqual('DefrostHsmState_PrechillPrep');
    });
  });

  xtest('should be idle', async () => {
    await board.expect('Erd_DefrostHsmState').toEqual('DefrostHsmState_Idle');
  });

  xtest('should pass', async () => {
    const count = await board.read('Erd_SecondsSinceLastReset');
    await board.expect('Erd_SecondsSinceLastReset').not.toEqual(count + 1);
    await delay(1000);
    await board.expect('Erd_SecondsSinceLastReset').toEqual(count + 1);
  });

  xtest('should log', async () => {
    const tracker = Tracker('Erd_SecondsSinceLastReset', 500, true);
    await delay(5000);
    tracker.pause();
    await delay(1000);
    tracker.start();
    await delay(2000);
    tracker.pause(); // important, or it'll keep running (unless you want that, then move into describe block)
  });

  xtest('should fail', async () => {
    const count = await board.read('Erd_SecondsSinceLastReset');
    await board.expect('Erd_SecondsSinceLastReset').toEqual(count + 1);
  });
});
