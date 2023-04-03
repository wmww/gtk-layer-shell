#!/usr/bin/python3
'''
This entire file is licensed under MIT.

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# Tests need to be added to meson.build. This script makes sure they are.

import os
from os import path
import logging

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.WARNING)

dead_tests = []

def check_dir(dir_path):
    logger.info('checking ' + dir_path)
    assert path.isdir(dir_path)
    meson_path = path.join(dir_path, 'meson.build')
    assert path.isfile(meson_path)
    with open(meson_path, 'r') as f:
        meson = f.read()
    for filename in os.listdir(dir_path):
        root, ext = path.splitext(filename)
        if ext == '.c':
            search_str = "'" + root + "'"
            if search_str in meson:
                logger.info(search_str + ' is in meson')
            else:
                logger.info(search_str + ' is not in meson')
                dead_tests.append(path.join(dir_path, filename))
        else:
            logger.info(filename + ' ignored')

if __name__ == '__main__':
    test_dir = path.dirname(path.realpath(__file__))
    check_dir(path.join(test_dir, 'integration-tests'))
    check_dir(path.join(test_dir, 'unit-tests'))
    if dead_tests:
        print('The following tests have not been added to meson:')
        for test in dead_tests:
            print('  ' + test)
        exit(1)
