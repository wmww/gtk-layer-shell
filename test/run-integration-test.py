#!/usr/bin/python3
'''
This entire file is licensed under MIT.

Copyright 2020 Sophie Winter

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
'''

# This script runs an integration test. See test/README.md for details
usage = 'Usage: python3 run-test <test-binary>'

import os
from os import path
import sys
import shutil
import time
import subprocess
import threading
from typing import List, Dict, Optional, Any

# All callables (generally lambdas) appended to this list will be called at the end of the program
cleanup_funcs = []

class TestError(RuntimeError):
    pass

def get_xdg_runtime_dir() -> str:
    '''
    Creates a directory to use as the XDG_RUNTIME_DIR.
    It bases the result on the current PID because each test running in parallel needs a unique directory.
    '''
    tmp_runtime_dir = '/tmp/layer-shell-test-runtime-dir-' + str(os.getpid())
    if (path.exists(tmp_runtime_dir)):
        # We should wipe the dir on cleanup, but things can go wrong
        wipe_xdg_runtime_dir(tmp_runtime_dir)
    os.mkdir(tmp_runtime_dir)
    cleanup_funcs.append(lambda: wipe_xdg_runtime_dir(tmp_runtime_dir))
    return tmp_runtime_dir

def wipe_xdg_runtime_dir(p: str):
    assert p.startswith('/tmp'), 'Sanity check'
    assert 'layer-shell-test-runtime-dir' in p, 'Sanity check'
    shutil.rmtree(p)

def wait_until_appears(p: str):
    '''Waits for something to appear at the given path'''
    sleep_time = 0.01
    timeout = 5.0
    for i in range(int(timeout / sleep_time)):
        if path.exists(p):
            return
        time.sleep(0.01)
    raise TestError(p + ' did not appear in ' + str(timeout) + ' seconds')

def format_stream(name: str, stream: str) -> str:
    '''
    After collecting a programs output stream into a string, this function formats it for easy reading.
    Specifically, it gives it a colored border and a name.
    '''
    l_pad = 28 - len(name) // 2
    r_pad = l_pad
    if len(name) % 2 == 1:
        r_pad -= 1
    l_pad = max(l_pad, 1)
    r_pad = max(r_pad, 1)
    header = '─' * l_pad + '┤ ' + name + ' ├' + '─' * r_pad + '┈'
    divider = '\n│'
    body = divider.join(' ' + line for line in stream.strip().splitlines())
    footer = '─' * 60 + '┈'
    return '╭' + header + divider + body + '\n╰' + footer

def format_process_report(name: str, returncode: int, stdout: str, stderr: str) -> str:
    '''After running a program, this function is used to format it's output for easy reading'''
    result = format_stream(name + ' stderr', stderr) + '\n\n'
    if stdout:
        result += format_stream(name + ' stdout', stdout) + '\n\n'
    else:
        result += 'stdout empty, '
    result += 'exit code: ' + str(returncode)
    return result

class Pipe:
    '''
    The normal Python subprocess.PIPE freezes the subprocess after it fills up a finite output buffer.
    This class solves that by opening a pipe and reading from it from another thread whil the subprocess runs.
    This increases the output limit to the system's available memory

    See: https://github.com/wmww/gtk-layer-shell/issues/91#issuecomment-719082062
    See: https://thraxil.org/users/anders/posts/2008/03/13/Subprocess-Hanging-PIPE-is-your-enemy/
    '''
    def __init__(self, name: str):
        '''name is only for debugging'''
        readable, writable = os.pipe()
        self.fd = writable
        self.data: Any = bytes()
        self.result: Optional[str] = None
        # Read the data coming out of the pipe on a background thread
        # This keeps the buffer from filling up and blocking
        self.reader_thread = threading.Thread(name=name, target=self.read, args=(readable,))
        self.reader_thread.start()
        cleanup_funcs.append(lambda: self.close())

    def read(self, readable: int):
        '''Reads from the given fd until the other side closes'''
        while True:
            data = os.read(readable, 1000)
            if not data:
                # We've reached the end of the data
                os.close(readable)
                return
            self.data += data

    def close(self):
        '''Closes the fd and stops the reader'''
        if self.reader_thread.is_alive():
            os.close(self.fd)
            self.reader_thread.join(timeout=1)
            assert not self.reader_thread.is_alive(), 'Failed to join pipe reader thread'

    def collect_str(self) -> str:
        '''Closes the pipe if needed and returns the read data decoded as UTF-8'''
        if self.result is None:
            self.close()
            self.result = self.data.decode('utf-8')
            self.data = None
        return self.result

class Program:
    '''A program to run as a subprocess'''
    def __init__(self, name: str, args: List[str], env: Dict[str, str]):
        self.name = name
        self.stdout = Pipe(name + ' stdout')
        self.stderr = Pipe(name + ' stderr')
        self.subprocess = subprocess.Popen(args, stdout=self.stdout.fd, stderr=self.stderr.fd, env=env)
        cleanup_funcs.append(lambda: self.kill())

    def finish(self, timeout: float):
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
        return format_process_report(
            self.name,
            self.subprocess.returncode,
            self.stdout.collect_str(),
            self.stderr.collect_str())

    def check_returncode(self):
        assert self.subprocess.returncode is not None, repr(self.name) + '.check_returncode() called before process exited'
        if self.subprocess.returncode != 0:
            raise TestError(
                self.format_output() + '\n\n' +
                self.name + ' failed (return code ' + str(self.subprocess.returncode) + ')')

    def collect_output(self):
        return self.stdout.collect_str(), self.stderr.collect_str()

def run_test(name: str, server_args: List[str], client_args: List[str], xdg_runtime: str, wayland_display: str) -> str:
    '''
    Runs two processes: a mock server and the test client
    Does *not* check that client's message assertions pass, this must be done later using the returned output
    '''
    env = os.environ.copy()
    env['XDG_RUNTIME_DIR'] = xdg_runtime
    env['WAYLAND_DISPLAY'] = wayland_display
    env['WAYLAND_DEBUG'] = '1'

    server = Program('server', server_args, env)

    try:
        wait_until_appears(path.join(xdg_runtime, wayland_display))
    except TestError as e:
        server.kill()
        raise TestError(server.format_output() + '\n\n' + str(e))

    client = Program(name, client_args, env)
    client.finish(timeout=10)
    server.finish(timeout=1)

    server.check_returncode()
    client.check_returncode()

    client_stdout, client_stderr = client.collect_output()

    if client_stdout.strip() != '':
        raise TestError(format_stream(name + ' stdout', client_stdout) + '\n\n' + name + ' stdout not empty')

    return client_stderr

def line_contains(line: str, tokens: List[str]) -> bool:
    '''Returns if the given line contains a list of tokens in the given order (anything can be between tokens)'''
    found = True
    for token in tokens:
        if token in line:
            line = line[line.find(token) + len(token):]
        else:
            found = False
    return found

def verify_result(lines: List[str]):
    '''Runs through the output of a client and verifies that all expectations pass, see the test README.md details'''
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
    client_bin = sys.argv[1]
    name = path.basename(client_bin)
    server_bin = path.join(path.dirname(client_bin), 'mock-server', 'mock-server')
    assert path.exists(client_bin), 'Could not find client at ' + client_bin
    assert path.exists(server_bin), 'Could not find server at ' + server_bin
    wayland_display = 'wayland-test'
    xdg_runtime = get_xdg_runtime_dir()

    client_stderr = run_test(name, [server_bin], [client_bin, '--auto'], xdg_runtime, wayland_display)
    client_lines = [line.strip() for line in client_stderr.strip().splitlines()]

    try:
        verify_result(client_lines)
    except TestError as e:
        raise TestError(format_stream(name + ' stderr', client_stderr) + '\n\n' + str(e))

if __name__ == '__main__':
    assert len(sys.argv) == 2, 'Incorrect number of args. ' + usage
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
