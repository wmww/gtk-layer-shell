mock_server_srcs = files(
    'mock-server.h',
    'mock-server.c',
    'overrides.c')

mock_server = executable(
    'mock-server',
    mock_server_srcs, server_protocol_srcs,
    c_args: ['-Wno-unused-parameter'],
    dependencies: [wayland_server, test_common])
