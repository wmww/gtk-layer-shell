mock_server = executable(
    'mock-server',
    ['mock-server.c'], server_protocol_srcs,
    c_args: ['-Wno-unused-parameter'],
    dependencies: [wayland_server])
