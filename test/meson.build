subdir('mock-server')

test_clients = [
    'basic-test',
    'test-anchors',
]
test_client_common_inc = include_directories('test-client-common')
py = find_program('python3')


foreach test_client : test_clients
    srcs = [
        'test-client-common/test-client-common.h',
        'test-client-common/test-client-common.c',
        'test-clients/' + test_client + '.c',
    ]
    exe = executable(
        test_client,
        srcs,
        dependencies: [gtk],
        include_directories: [gtk_layer_shell_inc, test_client_common_inc],
        link_with: [gtk_layer_shell_lib])
    test(
        test_client,
        py,
        workdir: meson.current_source_dir(),
        args: [
            meson.current_source_dir() + '/run-test.py',
            meson.current_build_dir(),
            test_client
        ])
endforeach

test('check-licenses', py, args: [meson.current_source_dir() + '/check-licenses.py'])
