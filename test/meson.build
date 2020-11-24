subdir('test-common')
subdir('mock-server')
subdir('integration-test-common')
subdir('integration-tests')
subdir('unit-tests')

py = find_program('python3')
run_test_script = files(meson.current_source_dir() + '/run-integration-test.py')

foreach integration_test : integration_tests
    integration_test_srcs = files('integration-tests/' + integration_test + '.c')
    exe = executable(
        integration_test,
        integration_test_srcs,
        dependencies: [gtk, wayland_client, gtk_layer_shell, integration_test_common])
    expect_fail = integration_test.endswith('expect-fail')
    test(
        integration_test,
        py,
        should_fail: expect_fail,
        workdir: meson.current_source_dir(),
        args: [
            run_test_script,
            meson.current_build_dir() + '/' + integration_test,
        ])
endforeach

check_licenses_script = files(meson.current_source_dir() + '/check-licenses.py')
test('check-licenses', py, args: [check_licenses_script])

check_tests_in_meson_script = files(meson.current_source_dir() + '/check-all-tests-are-in-meson.py')
test('check-all-tests-are-in-meson', py, args: [check_tests_in_meson_script])
