#!/bin/sh
set -e
if test "$CONFIGURATION" = "Debug"; then :
  cd /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build
  make -f /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "Release"; then :
  cd /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build
  make -f /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "MinSizeRel"; then :
  cd /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build
  make -f /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build/CMakeScripts/ReRunCMake.make
fi
if test "$CONFIGURATION" = "RelWithDebInfo"; then :
  cd /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build
  make -f /Users/roger/Data/Projects/Strategic/multi-platform-empty/playground/build/CMakeScripts/ReRunCMake.make
fi

