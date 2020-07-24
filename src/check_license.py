#!/usr/bin/python3
'''
This entire file is licensed under MIT.

Copyright 2020 William Wold

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import logging
import re
import os
from os import path
import subprocess
from fnmatch import fnmatch

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

IGNORE_PATTERNS = [
    '*/.git',
    '*/.gitignore',
    '*/.github',
    '*/doc',
    '*.md',
    '*.editorconfig',
    '*.txt',
    '*/src/protocol',
    '*/check_license.py',
    '*/meson.build',
    '*/gtk-priv/scripts/code.py', # Is MIT but has the LGPL license text in a string
]

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

def ignored_by_git(file_path):
    result = subprocess.run(
        ['git','-C', get_project_root(), 'check-ignore', file_path],
        stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return result.returncode == 0

def get_files(search_path):
    for pattern in IGNORE_PATTERNS:
        if fnmatch(search_path, pattern):
            logger.info(search_path + ' explicitly ignored (' + pattern + ')')
            return []
    if ignored_by_git(search_path):
        logger.info(search_path + ' ignored by git')
        return []
    if search_path.endswith('/build.ninja'):
        raise RuntimeError('Should not have tried to search a build dir')

    if path.isfile(search_path):
        logger.info('Found ' + search_path)
        return [search_path]
    elif path.isdir(search_path):
        logger.info('Scanning ' + search_path)
        result = []
        for item in os.listdir(search_path):
            result += get_files(path.join(search_path, item))
        return result

def get_important_files():
    return get_files(get_project_root())

def print_list(name, files):
    if files:
        print('The following files are licensed under ' + name + ':')
        for f in files:
            print('  ' + f)
    else:
        print('No files are licensed under ' + name)
    print()

def main():
    logger.info('Project root: ' + get_project_root())
    all_files = get_important_files()
    logger.info('Found ' + str(len(all_files)) + ' files')
    mit_files = []
    lgpl3_files = []
    none_files = []
    multiples_files = []
    mit_example = canonify_str(MIT_EXAMPLE)
    lgpl3_example = canonify_str(LGPL3_EXAMPLE)
    for p in all_files:
        with open(p, 'r') as f:
            contents = canonify_str(f.read())
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
        print('If some files should be excluded from the license check, add them to IGNORE_PATTERNS in ' + __file__)
        print('Failed license check')
        exit(1)
    else:
        print('Passed license check')
        exit(0)

if __name__ == '__main__':
    main()