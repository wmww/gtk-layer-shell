#!/bin/sh

autoreconf --verbose --force --install && ./configure "$@"
