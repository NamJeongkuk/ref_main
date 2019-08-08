#!/usr/bin/env bash

OPERATING_SYSTEM=`uname`

if [[ $OPERATING_SYSTEM == 'Linux' ]]; then
  MAKE=make
elif [[ $OPERATING_SYSTEM == 'Darwin' ]]; then
  MAKE=dmake
else
  echo 'Unsupported operating system!'
  exit 1
fi

$MAKE -f truk-target.mk -j8 RELEASE=N DEBUG=Y package
