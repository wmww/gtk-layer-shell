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

def get_xdg_runtime_dir():
    tmp_runtime_dir = '/tmp/layer-shell-test-runtime-dir-' + str(os.getpid())
    if (path.exists(tmp_runtime_dir)):
        wipe_xdg_runtime_dir(tmp_runtime_dir)
    os.mkdir(tmp_runtime_dir)
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
    raise RuntimeError(p + ' did not appear in ' + str(timeout) + ' seconds')

def format_stream(name, stream):
    l_pad = 18 - len(name) // 2
    r_pad = l_pad
    if len(name) % 2 == 1:
        r_pad += 1
    header = '─' * l_pad + '┤ ' + name + ' ├' + '─' * r_pad + '┈'
    body = '\n│'.join('  ' + line for line in stream.strip().splitlines())
    footer = '─' * 40 + '┈'
    return '╭' + header + '\n│\n│' + body + '\n│\n╰' + footer

def run_test_processess(name, server_bin, client_bin, xdg_runtime, wayland_display):
    server = subprocess.Popen(
        server_bin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env={
            'XDG_RUNTIME_DIR': xdg_runtime,
            'WAYLAND_DISPLAY': wayland_display,
            'WAYLAND_DEBUG': '1',
        })

    try:
        wait_until_appears(path.join(xdg_runtime, wayland_display))
    except RuntimeError as e:
        server_streams = [stream.decode('utf-8') for stream in server.communicate()]
        report = '\n\n'.join([
            format_stream('server stdout', server_streams[0]),
            format_stream('server stderr', server_streams[1]),
            'server exit code: ' + str(server.returncode),
        ])
        print(report)
        print()
        print('server failed')
        exit(1)

    client = subprocess.Popen(
        client_bin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env={
            'XDG_RUNTIME_DIR': xdg_runtime,
            'WAYLAND_DISPLAY': wayland_display,
            'WAYLAND_DEBUG': '1',
        })

    client_streams = [stream.decode('utf-8') for stream in client.communicate(timeout=5)]
    server_streams = [stream.decode('utf-8') for stream in server.communicate(timeout=1)]

    report = '\n\n'.join([
        format_stream(name + ' stdout', client_streams[0]),
        format_stream(name + ' stderr', client_streams[1]),
        name + ' exit code: ' + str(client.returncode),
        format_stream('server stdout', server_streams[0]),
        format_stream('server stderr', server_streams[1]),
        'server exit code: ' + str(server.returncode),
    ])

    if server.returncode != 0:
        raise RuntimeError('!!! server failed !!!\n\n' + report + '\n\n!!! server failed !!!')

    if client.returncode != 0:
        raise RuntimeError('!!! ' + name + ' failed !!!\n\n' + report + '\n\n!!! ' + name + ' failed !!!')

    return client_streams

def verify_result(assert_lines, log_lines):
    i = 0
    for assertion in assert_lines:
        possible_matches = []
        found = False
        while not found:
            if i >= len(log_lines):
                possible_matches = format_stream('incomplete matches', '\n'.join(possible_matches))
                assertion = ' '.join(assertion)
                message = possible_matches + '\n\n' + 'failed to find "' + assertion + '"'
                raise RuntimeError(message)
            assert assertion[0] == 'WL:', '"' + assertion + '" does not start with WL:'
            partial = False
            found = True
            line = log_lines[i]
            for token in assertion[1:]:
                if token in line:
                    if len(token) > 2:
                        partial = True
                    line = line[line.find(token) + len(token):]
                else:
                    found = False
            if partial:
                possible_matches.append(log_lines[i])
            i += 1

def run_test(name):
    server_bin = get_bin('mock-server/mock-server')
    client_bin = get_bin(name)
    wayland_display = 'wayland-test'
    xdg_runtime = get_xdg_runtime_dir()

    try:
        client_stdout, client_stderr = run_test_processess(name, server_bin, client_bin, xdg_runtime, wayland_display)
    finally:
        wipe_xdg_runtime_dir(xdg_runtime)

    assert_lines = []
    for line in client_stdout.strip().splitlines():
        line = line.strip()
        if line:
            if not line.startswith('WL: '):
                print(format_stream('assertions', client_stdout))
                print()
                print('Invalid assertion line: "' + line + '" does not start with WL:')
                print('There should be no unexpected stdout output')
                exit(1)
            assert_lines.append(line.split())

    log_lines = []
    for line in client_stderr.splitlines():
        line = line.strip()
        if line:
            if not line.startswith('[') or not line.endswith(')'):
                print(format_stream('messages', client_stderr))
                print()
                print('Invalid stderr line: ' + line)
                print('There should be no unexpected stderr output')
            log_lines.append(line)

    try:
        verify_result(assert_lines, log_lines)
    except Exception as e:
        print(format_stream('assertions', client_stdout))
        print()
        print(format_stream('messages', client_stderr))
        print()
        print(e)
        exit(1)

if __name__ == '__main__':
    assert len(sys.argv) == 3, 'Incorrect number of args. ' + usage
    run_test(sys.argv[2])
    print('Passed')
