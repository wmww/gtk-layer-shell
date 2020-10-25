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

def decode_streams(streams):
    return [stream.decode('utf-8') for stream in streams]

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

def format_process_report(name, process, streams):
    streams = (
        format_stream(name + ' stdout', streams[0]) + '\n\n',
        format_stream(name + ' stderr', streams[1]) + '\n\n')
    if name == 'server':
        streams = (streams[1], streams[0])
    return ''.join(streams) + name + ' exit code: ' + str(process.returncode)

def run_test(name, server_bin, client_bin, xdg_runtime, wayland_display):
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
        server.kill()
        server_streams = decode_streams(server.communicate())
        raise RuntimeError(format_process_report('server', server, server_streams) + '\n\n' + str(e))

    client = subprocess.Popen(
        client_bin,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env={
            'XDG_RUNTIME_DIR': xdg_runtime,
            'WAYLAND_DISPLAY': wayland_display,
            'WAYLAND_DEBUG': '1',
        })

    try:
        client_streams = decode_streams(client.communicate(timeout=5))
    except subprocess.TimeoutExpired:
        client.kill()
        time.sleep(1)
        server.kill()
        client_streams = decode_streams(client.communicate())
        server.communicate(timeout=1)
        raise RuntimeError(format_process_report(name, client, client_streams) + '\n\n' + name + ' timed out')

    try:
        server_streams = decode_streams(server.communicate(timeout=1))
    except subprocess.TimeoutExpired:
        server.kill()
        server_streams = decode_streams(server.communicate())
        raise RuntimeError(format_process_report('server', server, server_streams) + '\n\nserver timed out')

    if server.returncode != 0:
        raise RuntimeError(format_process_report('server', server, server_streams) + '\n\nserver failed')

    if client.returncode != 0:
        raise RuntimeError(format_process_report(name, client, client_streams) + '\n\n' + name + ' failed')

    if client_streams[0].strip() != '':
        raise RuntimeError(format_stream(name + ' stdout', client_streams[0]) + '\n\n' + name + ' stdout not empty')

    return client_streams[1]

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
                raise RuntimeError(section + '\n\ndid not find "' + ' '.join(assertions[0]) + '"')
            section_start = i + 1

if __name__ == '__main__':
    assert len(sys.argv) == 3, 'Incorrect number of args. ' + usage
    name = sys.argv[2]
    try:
        server_bin = get_bin('mock-server/mock-server')
        client_bin = get_bin(name)
        wayland_display = 'wayland-test'
        xdg_runtime = get_xdg_runtime_dir()

        try:
            client_stderr = run_test(name, server_bin, client_bin, xdg_runtime, wayland_display)
        finally:
            wipe_xdg_runtime_dir(xdg_runtime)

        client_lines = [line.strip() for line in client_stderr.strip().splitlines()]
        try:
            verify_result(client_lines)
        except RuntimeError as e:
            raise RuntimeError(format_stream(name + ' stderr', client_stderr) + '\n\n' + str(e))

        print('Passed')
    except RuntimeError as e:
        print(e)
        exit(1)
