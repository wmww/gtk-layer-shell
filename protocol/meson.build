if wayland_scanner.found()
    prog_wayland_scanner = find_program(wayland_scanner.get_pkgconfig_variable('wayland_scanner'))
else
    prog_wayland_scanner = find_program('wayland-scanner')
endif

protocols = [
    'wlr-layer-shell-unstable-v1.xml'
]

if wayland_protocols.found()
    protocols += join_paths(
        wayland_protocols.get_pkgconfig_variable('pkgdatadir'),
        'stable/xdg-shell/xdg-shell.xml')
else
    # use bundled xdg-shell.xml
    protocols += 'xdg-shell.xml'
endif

gen_client_header = generator(prog_wayland_scanner,
    output: ['@BASENAME@-client.h'],
    arguments: ['-c', 'client-header', '@INPUT@', '@BUILD_DIR@/@BASENAME@-client.h'])

gen_server_header = generator(prog_wayland_scanner,
    output: ['@BASENAME@-server.h'],
    arguments: ['-c', 'server-header', '@INPUT@', '@BUILD_DIR@/@BASENAME@-server.h'])

gen_private_code = generator(prog_wayland_scanner,
    output: ['@BASENAME@.c'],
    arguments: ['-c', 'code', '@INPUT@', '@BUILD_DIR@/@BASENAME@.c'])
# 'code' is deprecated, and can be replaced with 'private-code' when all platforms have a new enough wayland-scanner

client_protocol_srcs = []
server_protocol_srcs = []

foreach protocol : protocols
    client_header = gen_client_header.process(protocol)
    server_header = gen_server_header.process(protocol)
    code = gen_private_code.process(protocol)
    client_protocol_srcs += [client_header, code]
    server_protocol_srcs += [server_header, code]
endforeach
