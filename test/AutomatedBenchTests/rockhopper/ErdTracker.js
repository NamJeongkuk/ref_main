'use strict';

const board = require('../setup/board');
const { inspect } = require('util');

module.exports = (erd, ms, log) => {
  let data = [];
  let interval;

  const print = () => console.log(inspect(data));

  const Interval = () =>
    setInterval(async () => {
      data.push(await board.read(erd));
      if (log) {
        print();
      }
    }, ms);

  return {
    pause: () => {
      clearInterval(interval);
    },
    start: () => {
      interval = Interval();
    },
    getData: () => data,
    clear: () => (data = []),
    print,
  };
};
