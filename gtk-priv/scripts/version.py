'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import logging
import functools
import re

from config import MIN_SUPPORTED_GTK, MAX_SUPPORTED_GTK

logger = logging.getLogger(__name__)

COMBO_FACTOR = 1000

@functools.total_ordering # implement all comparisons with just eq and lt
class Version:
    def __init__(self, git_name, minor, micro, released):
        self.git_name = git_name
        self.minor = minor
        self.micro = micro
        self.released = released

    def __eq__(self, other):
        return self.minor == other.minor and self.micro == other.micro

    def __lt__(self, other):
        if self.minor == other.minor:
            return self.micro < other.micro
        else:
            return self.minor < other.minor

    def get_checkout_name(self):
        return self.git_name

    def is_supported(self):
        '''Returns if the version is one we support'''
        return (
            self >= min_supported_version and
            self <= max_supported_version and
            self != bad_release_3_24_19)

    def is_released(self):
        return self.released

    def get_combo(self):
        return self.minor * 1000 + self.micro

    def __str__(self):
        result = 'v3.' + str(self.minor) + '.' + str(self.micro)
        if not self.is_released():
            result += ' (unreleased)'
        return result

    def c_id(self):
        '''a string suitable for a C identifier'''
        return 'v3_' + str(self.minor) + '_' + str(self.micro)

def parse_tag(tag):
    match = re.search(r'^3\.(\d+)\.(\d+)$', tag)
    if match:
        minor = int(match.group(1))
        micro = int(match.group(2))
        return Version(tag, minor, micro, True)
    else:
        return None

def parse_branch(branch):
    match = re.search(r'^gtk\-3\-(\d+)$', branch)
    if match:
        minor = int(match.group(1))
        return minor
    else:
        return None

min_supported_version = parse_tag(MIN_SUPPORTED_GTK)
max_supported_version = parse_tag(MAX_SUPPORTED_GTK)
bad_release_3_24_19 = parse_tag('3.24.19') # this is not a good release

def parse_tags_and_branches(tags, branches):
    result = []
    for tag in tags:
        version = parse_tag(tag)
        if version and version.is_supported():
            result.append(version)
    highest_micro = {}
    for v in result:
        highest_micro[v.minor] = max(v.micro, highest_micro.get(v.minor, -1))
    for branch in branches:
        minor = parse_branch(branch)
        if minor is not None:
            micro = highest_micro.get(minor, -1) + 1
            version = Version(branch, minor, micro, False)
            if version.is_supported():
                result.append(version)
    result.sort()
    logger.info('Found ' + str(len(result)) + ' supported versions')
    return result
