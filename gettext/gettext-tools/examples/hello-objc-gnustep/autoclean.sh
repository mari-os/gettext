#!/bin/sh
# Example for use of GNU gettext.
# Copyright (C) 2003 Free Software Foundation, Inc.
# This file is in the public domain.
#
# Script for cleaning all autogenerated files.

make distclean

rm -f po/remove-potcdate.sed
rm -f po/remove-potcdate.sin
rm -f po/*.pot
rm -rf *.lproj
