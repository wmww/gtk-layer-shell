if wayland_scanner.found()
    prog_wayland_scanner = find_program(wayland_scanner.get_pkgconfig_variable('wayland_scanner'))
else
    prog_wayland_scanner = find_program('wayland-scanner')
endif

gen_client_header = generator(prog_wayland_scanner,
    output: ['@BASENAME@-client.h'],
    arguments: ['-c', 'client-header', '@INPUT@', '@BUILD_DIR@/@BASENAME@-client.h'])

gen_private_code = generator(prog_wayland_scanner,
    output: ['@BASENAME@.c'],
    arguments: ['-c', 'code', '@INPUT@', '@BUILD_DIR@/@BASENAME@.c'])
# 'code' is deprecated, and can be replaced with 'private-code' when all platforms have a new enough wayland-scanner

xdg_shell_client_header = gen_client_header.process('xdg-shell.xml')
xdg_shell_private_code = gen_private_code.process('xdg-shell.xml')

layer_shell_client_header = gen_client_header.process('wlr-layer-shell-unstable-v1.xml')
layer_shell_private_code = gen_private_code.process('wlr-layer-shell-unstable-v1.xml')
