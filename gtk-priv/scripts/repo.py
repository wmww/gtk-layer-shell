'''
MIT License

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

import subprocess
import logging
from os import path

logger = logging.getLogger(__name__)

class Repo:
    def __init__(self, repo_url, repo_dir):
        self.repo_dir = repo_dir
        if not path.exists(self.repo_dir):
            logger.info('Cloning ' + repo_url + ' into ' + self.repo_dir)
            subprocess.run(
                ['git', 'clone', repo_url, self.repo_dir]
                ).check_returncode()
        else:
            logger.info('Fetching')
            subprocess.run(
                ['git', 'fetch'],
                cwd=self.repo_dir
                ).check_returncode()

    def get_branches(self):
        result = subprocess.run(
            ['git', 'branch', '-r'],
            capture_output=True,
            encoding='utf-8',
            cwd=self.repo_dir)
        result.check_returncode()
        branches = result.stdout.splitlines()
        # strip of origin/
        branches = [branch.split('/')[-1] for branch in branches]
        logger.info('Found ' + str(len(branches)) + ' git branches')
        return branches

    def get_tags(self):
        '''Returns a list of git tags'''
        result = subprocess.run(
            ['git', 'tag', '-l'],
            capture_output=True,
            encoding='utf-8',
            cwd=self.repo_dir)
        result.check_returncode()
        tags = result.stdout.splitlines()
        logger.info('Found ' + str(len(tags)) + ' git tags')
        return tags

    def checkout(self, tag):
        '''Checks out a branch or tag'''
        # logger.info('Checking out ' + tag)
        result = subprocess.run(
            ['git', 'checkout', tag],
            cwd=self.repo_dir,
            capture_output=True
            ).check_returncode()
