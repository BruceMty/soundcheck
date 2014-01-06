#!/bin/sh
/bin/rm -rf aclocal.m4
autoheader -f
aclocal -I m4

# libtoolize is required
if $(type -p libtoolize); then
  libtoolize
elif  $(type -p glibtoolize); then
  glibtoolize
else
  echo "Please install libtoolize or glibtoolize to run this script"
fi

autoconf -f
automake --add-missing --copy

