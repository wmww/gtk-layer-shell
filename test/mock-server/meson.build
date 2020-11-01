mock_server_srcs = [
    'mock-server.h',
    'mock-server.c',
    'overrides.c',
]

mock_server = executable(
    'mock-server',
    mock_server_srcs, server_protocol_srcs,
    c_args: ['-Wno-unused-parameter'],
    include_directories: [test_common_inc],
    dependencies: [wayland_server])
