#!/usr/bin/python3
'''
This entire file is licensed under MIT.

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# This script checks that every soruce file in the project has an appropriate license

import logging
import re
import os
from os import path
import subprocess
import re

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.WARNING)

toplevel_dirs = ['include', 'src', 'gtk-priv', 'examples', 'test']
ignore_patterns_file = 'test/license-ignore.txt'

MIT_EXAMPLE = '''
Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

LGPL3_EXAMPLE = '''
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program; if not, write to the Free Software Foundation,
Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
'''

def canonify_str(s):
    return re.sub(r'[/#\'\n\s\*]', '', s)

def get_project_root():
    return path.dirname(path.dirname(path.realpath(__file__)))

ignore_patterns = None
def get_ignore_patterns():
    global ignore_patterns
    if ignore_patterns is not None:
        return ignore_patterns
    ignore_patterns = []
    for f in ['.gitignore', '.git/info/exclude', ignore_patterns_file]:
        p = get_project_root() + '/' + f
        if path.isfile(p):
            logger.info('Excluding paths in ' + p + ' from the license check')
            for raw_line in open(p, 'r').read().splitlines():
                line = re.sub(r'([^#]*)(.*)', r'\1', raw_line).strip()
                if line:
                    ignore_patterns.append(line)
        else:
            logger.warning(p + ' not found, it will not be used to exclude paths from the license check')
    return ignore_patterns

def path_matches(base_path, original):
    pattern = re.escape(original)
    pattern = re.sub(r'\\\*', r'.*', pattern)
    pattern = re.sub(r'\\?/', r'(^|$|/)', pattern)
    try:
        return bool(re.match(pattern, base_path))
    except Exception as e:
        raise RuntimeError('Failed to match pattern ' + pattern + ' (original: ' + original + ') against ' + base_path + ': ' + str(e))

def get_files(prefix, search_path):
    full_path = path.join(prefix, search_path);
    assert path.exists(full_path), full_path + ' does not exist'
    if path.exists(path.join(full_path, 'build.ninja')):
        logger.info(search_path + ' ignored because it is a build directory')
        return []
    for pattern in get_ignore_patterns():
        if path_matches(search_path, pattern):
            logger.info(search_path + ' ignored because it matches ' + pattern)
            return []
    if path.isfile(full_path):
        logger.info('Found ' + search_path)
        return [search_path]
    elif path.isdir(full_path):
        logger.info('Scanning ' + search_path)
        result = []
        for item in os.listdir(full_path):
            result += get_files(prefix, path.join(search_path, item))
        return result
    else:
        return []

def get_important_files():
    result = []
    for toplevel_dir in toplevel_dirs:
        logger.info('Scanning toplevel directory ' + toplevel_dir)
        result += get_files(get_project_root(), toplevel_dir)
    return result

def print_list(name, files):
    if files:
        print('The following files are licensed under ' + name + ':')
        for f in files:
            print('  ' + f)
    else:
        print('No files are licensed under ' + name)
    print()

def load_file(p):
    try:
        with open(p, 'r') as f:
            contents = f.read()
            return canonify_str(contents)
    except Exception:
        rel = path.relpath(p, get_project_root())
        raise RuntimeError(
            'Failed to read ' + rel +
            '. If this file should have been ignored, add it to ' + ignore_patterns_file)

def main():
    logger.info('Project root: ' + get_project_root())
    logger.info('Ignore paths: \n  ' + '\n  '.join(get_ignore_patterns()))
    all_files = get_important_files()
    logger.info('Found ' + str(len(all_files)) + ' files')
    assert len(all_files) > 10, 'There are ' + str(len(all_files)) + ' files (which is not as many as there should be)'
    mit_files = []
    lgpl3_files = []
    none_files = []
    multiples_files = []
    mit_example = canonify_str(MIT_EXAMPLE)
    lgpl3_example = canonify_str(LGPL3_EXAMPLE)
    for p in all_files:
        contents = load_file(path.join(get_project_root(), p))
        found = 0
        if mit_example in contents:
            mit_files.append(p)
            found += 1
        if lgpl3_example in contents:
            lgpl3_files.append(p)
            found += 1
        if found > 1:
            multiples_files.append(p)
        elif found < 1:
            none_files.append(p)
    print()
    print_list('MIT', mit_files)
    print_list('LGPLv3', lgpl3_files)
    print_list('no license', none_files)
    print_list('multiple licenses', multiples_files)
    if none_files or multiples_files:
        print('If some files should be excluded from the license check, add them to ' + ignore_patterns_file)
        print('Failed license check')
        exit(1)
    else:
        print('Passed license check')
        exit(0)

if __name__ == '__main__':
    main()
