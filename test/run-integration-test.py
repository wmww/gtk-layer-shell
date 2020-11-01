#!/usr/bin/python3
'''
This entire file is licensed under MIT.

Copyright 2020 William Wold

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# This script runs an integration test. See test/README.md for details
usage = 'Usage: python3 run-test <test-build-dir> <test-name>'

import os
from os import path
import sys
import shutil
import time
import subprocess
import threading

cleanup_funcs = []

class TestError(RuntimeError):
    pass

def get_xdg_runtime_dir():
    tmp_runtime_dir = '/tmp/layer-shell-test-runtime-dir-' + str(os.getpid())
    if (path.exists(tmp_runtime_dir)):
        wipe_xdg_runtime_dir(tmp_runtime_dir)
    os.mkdir(tmp_runtime_dir)
    cleanup_funcs.append(lambda: wipe_xdg_runtime_dir(tmp_runtime_dir))
    return tmp_runtime_dir

def wipe_xdg_runtime_dir(p):
    assert p.startswith('/tmp'), 'Sanity check'
    assert 'layer-shell-test-runtime-dir' in p, 'Sanity check'
    shutil.rmtree(p)

def get_bin(p):
    build_dir = sys.argv[1]
    assert path.isdir(build_dir), build_dir + ' not a directory'
    p = path.join(build_dir, p)
    assert path.exists(p), p + ' does not exist'
    return p

def wait_until_appears(p):
    sleep_time = 0.01
    timeout = 5.0
    for i in range(int(timeout / sleep_time)):
        if path.exists(p):
            return
        time.sleep(0.01)
    raise TestError(p + ' did not appear in ' + str(timeout) + ' seconds')

def format_stream(name, stream):
    l_pad = 18 - len(name) // 2
    r_pad = l_pad
    if len(name) % 2 == 1:
        r_pad += 1
    l_pad = max(l_pad, 1)
    r_pad = max(r_pad, 1)
    header = '─' * l_pad + '┤ ' + name + ' ├' + '─' * r_pad + '┈'
    body = '\n│'.join('  ' + line for line in stream.strip().splitlines())
    footer = '─' * 40 + '┈'
    return '╭' + header + '\n│\n│' + body + '\n│\n╰' + footer

def format_process_report(name, process, stdout, stderr):
    streams = (
        format_stream(name + ' stdout', stdout) + '\n\n',
        format_stream(name + ' stderr', stderr) + '\n\n')
    if name == 'server':
        streams = (streams[1], streams[0])
    return ''.join(streams) + name + ' exit code: ' + str(process.returncode)

class Pipe:
    def __init__(self, name):
        readable, writable = os.pipe()
        self.fd = writable
        self.data = bytes()
        self.result = None
        # Read the data coming out of the pipe on a background thread
        # This keeps the buffer from filling up and blocking
        self.reader_thread = threading.Thread(name=name, target=self.read, args=(readable,))
        self.reader_thread.start()
        cleanup_funcs.append(lambda: self.close())

    def read(self, readable):
        while True:
            data = os.read(readable, 1000)
            if not data:
                # We've reached the end of the data
                break
            self.data += data
        os.close(readable)

    def close(self):
        if self.reader_thread.is_alive():
            os.close(self.fd)
            self.reader_thread.join(timeout=1)
            assert not self.reader_thread.is_alive(), 'Failed to join pipe reader thread'

    def collect_str(self):
        if self.result is None:
            self.close()
            self.result = self.data.decode('utf-8')
            data = None
        return self.result

class Program:
    def __init__(self, name, args, env):
        self.name = name
        self.stdout = Pipe(name + ' stdout')
        self.stderr = Pipe(name + ' stderr')
        self.subprocess = subprocess.Popen(args, stdout=self.stdout.fd, stderr=self.stderr.fd, env=env)
        cleanup_funcs.append(lambda: self.kill())

    def finish(self, timeout=None):
        try:
            self.subprocess.wait(timeout=timeout)
        except subprocess.TimeoutExpired:
            self.kill()
            raise TestError(self.format_output() + '\n\n' + self.name + ' timed out')

    def kill(self):
        if self.subprocess.returncode is None:
            self.subprocess.kill()
            self.subprocess.wait()

    def format_output(self):
        assert self.subprocess.returncode is not None, 'Program.format_output() called before process exited'
        return format_process_report(self.name, self.subprocess, self.stdout.collect_str(), self.stderr.collect_str())

    def check_returncode(self):
        assert self.subprocess.returncode is not None, repr(self.name) + '.check_returncode() called before process exited'
        if self.subprocess.returncode != 0:
            raise TestError(
                self.format_output() + '\n\n' +
                self.name + ' failed (return code ' + str(self.subprocess.returncode) + ')')

    def collect_output(self):
        return self.stdout.collect_str(), self.stderr.collect_str()

def run_test(name, server_bin, client_bin, xdg_runtime, wayland_display):
    env = os.environ.copy()
    env['XDG_RUNTIME_DIR'] = xdg_runtime
    env['WAYLAND_DISPLAY'] = wayland_display
    env['WAYLAND_DEBUG'] = '1'

    server = Program('server', server_bin, env)

    try:
        wait_until_appears(path.join(xdg_runtime, wayland_display))
    except TestError as e:
        server.kill()
        raise TestError(server.format_output() + '\n\n' + str(e))

    client = Program(name, client_bin, env)
    client.finish(timeout=10)
    server.finish(timeout=1)

    server.check_returncode()
    client.check_returncode()

    client_stdout, client_stderr = client.collect_output()

    if client_stdout.strip() != '':
        raise TestError(format_stream(name + ' stdout', client_stdout) + '\n\n' + name + ' stdout not empty')

    return client_stderr

def line_contains(line, tokens):
    found = True
    for token in tokens:
        if token in line:
            line = line[line.find(token) + len(token):]
        else:
            found = False
    return found

def verify_result(lines):
    assertions = []
    section_start = 0
    for i, line in enumerate(lines):
        if line.startswith('EXPECT: '):
            assertions.append(line.split()[1:])
        elif line.startswith('[') and line.endswith(')') and '@' in line:
            if assertions and line_contains(line, assertions[0]):
                assertions = assertions[1:]
        elif line == 'CHECK EXPECTATIONS COMPLETED' or i == len(lines) - 1:
            if assertions:
                section = format_stream('relevant section', '\n'.join(lines[section_start:i]))
                raise TestError(section + '\n\ndid not find "' + ' '.join(assertions[0]) + '"')
            section_start = i + 1

def main():
    name = sys.argv[2]
    server_bin = get_bin('mock-server/mock-server')
    client_bin = get_bin(name)
    wayland_display = 'wayland-test'
    xdg_runtime = get_xdg_runtime_dir()

    client_stderr = run_test(name, server_bin, client_bin, xdg_runtime, wayland_display)
    client_lines = [line.strip() for line in client_stderr.strip().splitlines()]

    try:
        verify_result(client_lines)
    except TestError as e:
        raise TestError(format_stream(name + ' stderr', client_stderr) + '\n\n' + str(e))

if __name__ == '__main__':
    assert len(sys.argv) == 3, 'Incorrect number of args. ' + usage
    fail = False
    try:
        main()
        print('Passed')
    except TestError as e:
        fail = True
        print(e)
    finally:
        for func in cleanup_funcs:
            func()
    if fail:
        exit(1)
