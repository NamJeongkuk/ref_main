#!/usr/bin/env bash
set -e

make -f bonzalez-target.mk -j8 RELEASE=N DEBUG=Y package
