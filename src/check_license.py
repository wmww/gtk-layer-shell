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

logger = logging.getLogger(__name__)
logging.basicConfig(level=logging.DEBUG)

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

def get_files(search_dir):
    result = []
    for item in os.listdir(search_dir):
        p = path.join(search_dir, item)
        if (item.startswith('.') or
            item == 'check_license.py' or
            item == 'meson.build' or
            item.endswith('.xml')):
            continue
        elif path.isdir(p):
            result += get_files(p)
        else:
            result.append(p)
    return result

def get_important_files():
    root = get_project_root()
    result = []
    result += get_files(path.join(root, 'src'))
    result += get_files(path.join(root, 'include'))
    result += get_files(path.join(root, 'example'))
    result += get_files(path.join(root, 'demo'))
    return result

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
        print('Failed license check')
        exit(1)
    else:
        print('Passed license check')
        exit(0)

if __name__ == '__main__':
    main()
