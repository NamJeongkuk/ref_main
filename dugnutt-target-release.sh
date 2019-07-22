OPERATING_SYSTEM=`uname`

if [[ $OPERATING_SYSTEM == Linux ]]; then
  MAKE=make
elif [[ $OPERATING_SYSTEM == Darwin ]]; then
  MAKE=dmake
else
  echo "Unsupported operating system!"
  exit 1
fi

make -f dugnutt-target.mk -j8 RELEASE=Y DEBUG=N package
