const ms = 1;
const msPerSec = 1000;
const msPerMin = 60 * msPerSec;
const secondsPerMin = 60;
const minutesPerHour = 60;
const bootLoaderResetDelay = 2 * msPerSec;
const disabled = false;
const enabled = true;
const off = false;
const on = true;
const abnormal = true;
const normal = false;
const set = true;
const clear = false;
const open = true;
const closed = false;
const INT16_MAX = 32767;
const INT16_MIN = -32768;

module.exports =
   Object.freeze({
      ms: ms,
      msPerSec: msPerSec,
      msPerMin: msPerMin,
      secondsPerMin: secondsPerMin,
      minutesPerHour: minutesPerHour,
      secondsPerHour: minutesPerHour * secondsPerMin,
      oneSecondInMsec: 1 * msPerSec,
      dataEntryTimeout: 30 * msPerSec,
      dataUpdateTimeout: 15 * msPerSec,
      bootLoaderResetDelay: bootLoaderResetDelay,
      disabled: disabled,
      enabled: enabled,
      off: off,
      on: on,
      abnormal: abnormal,
      normal: normal,
      set: set,
      clear: clear,
      open: open,
      closed: closed,
      INT16_MAX: INT16_MAX,
      INT16_MIN: INT16_MIN,
   });
