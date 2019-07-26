'use strict';

module.exports = {
  'dugnutt-target.mk': {
    ...require('./kpit-rx/dmake'),
    options: {
      ...require('./kpit-rx/dmake').options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  }
}
