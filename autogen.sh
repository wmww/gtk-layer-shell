#!/bin/sh
set -e

autoreconf --verbose --force --install
./configure "$@"
echo "Now run 'make' to build the project"
