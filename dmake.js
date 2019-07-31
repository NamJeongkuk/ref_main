'use strict';

module.exports = {
  'dugnutt-target.mk': {
    ...require('./tools/kpit-rx/dmake'),
    options: {
      ...require('./tools/kpit-rx/dmake').options,
      outputDirectories: [
        'build',
        'artifacts',
        'lib/boot-loaders/build'
      ]
    }
  },
  'truk-target.mk': {
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
