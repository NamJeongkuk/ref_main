'use strict';

const time = require('../support/timeAcceleration.js');

module.exports = async (t) => {
  await time.advance(t);
};
