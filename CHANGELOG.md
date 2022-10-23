# Changelog

## [Unreleased]

## [0.8.0] - 23 Oct 2022
- Vala: support generating vapi files
- Fix: memory leak, [#128](https://github.com/wmww/gtk-layer-shell/issues/128)
- Compat: stop warning when GTK version is not explicitly supported
- Compat: bump supported GTK to 3.24.34

## [0.7.0] - 25 Jan 2022
- Meson: allow disabling building with gobject-introspection
- Compat: bump supported GTK to v3.24.31

## [0.6.0] - 2 Mar 2021
- Layer Shell: support protocol version 4
- API: support on-demand keyboard interactivity on protocol version 4
- API: deprecate `gtk_layer_set_keyboard_interactivity()` in favor of `gtk_layer_set_keyboard_mode()`
- API: add `gtk_layer_get_protocol_version()`
- Compat: bump supported GTK to v3.24.26

## [0.5.2] - 11 Dec 2020
- Fix: normalize keyboard interactivity boolean to `TRUE` or `FALSE`
- Fix: Set layer windows to mapped (removes warning on tooltip creation and fixes subsurfaces created as a `GTK_WINDOW_POPUP` window, fixes [#38](https://github.com/wmww/gtk-layer-shell/issues/38))
- Examples: Rename `gtk-layer-example` -> `simple-example-c`
- Examples: merge `simple-example-c` and `gtk-layer-demo` into `examples` directory
- Examples: add Vala example (not built with rest of project, does not add dependency)
- Examples: add Python example
- Meson: change GTK dependency `gtk+-wayland-3.0` -> `gtk+-3.0`
- Meson: drop .types file in docs
- Tests: change "test client" terminology to "integration test"
- Tests: colored output
- Tests: check that all tests are added to meson
- Compat: bump supported GTK to v3.24.24

## [0.5.1] - 1 Nov 2020
- Tests: fix integration test timeout on GTK v3.24.23 (fixes [#91](https://github.com/wmww/gtk-layer-shell/issues/91))

## [0.5.0] - 28 Oct 2020
- API: add getters for all properties (fixes [#56](https://github.com/wmww/gtk-layer-shell/issues/56))
- API: add `gtk_layer_is_supported()` (fixes [#83](https://github.com/wmww/gtk-layer-shell/issues/83))
- API: add `gtk_layer_is_layer_window(window)`
- Docs: add `Since: ` annotations for functions added since the initial release (fixes [#82](https://github.com/wmww/gtk-layer-shell/issues/82))
- Fix: accept non-1 true boolean values (thanks [@alebastr](https://github.com/alebastr))
- Tests: fix license test for release tarball (fixes [#73](https://github.com/wmww/gtk-layer-shell/issues/73))
- Tests: test popup with grab (the most likely thing to break on an incompatible GTK version) (fixes [#68](https://github.com/wmww/gtk-layer-shell/issues/68))

## [0.4.0] - 21 Oct 2020
- Demo: show GTK Layer Shell version
- Demo: fix initially set auto-exclusive zone and fixed size properties (fixes [#64](https://github.com/wmww/gtk-layer-shell/issues/64))
- API: expose GTK Layer Shell version
- API: expose the underlying layer shell surface (fixes [#41](https://github.com/wmww/gtk-layer-shell/issues/41))
- Tests: add mock server
- Tests: add integration tests (fixes [#60](https://github.com/wmww/gtk-layer-shell/issues/60))
- Diagnostics: remove "transient_for != window transient_for" warning
- Diagnostics: improve warning when GTK version may not work by linking to a [compatibility table](https://github.com/wmww/gtk-layer-shell/blob/master/compatibility.md) (fixes [#66](https://github.com/wmww/gtk-layer-shell/issues/66))
- Compat: bump supported GTK to v3.24.23

## [0.3.0] - 12 Aug 2020
- Demo: Display GTK version
- Demo: Add option for disabling gtk-layer-shell for testing
- Fix intermittent crash relating to popovers ([#51](https://github.com/wmww/gtk-layer-shell/issues/51))
- Clarify that our license is LGPL version 3 or any later version
- Use system XDG Shell protocol when available (thanks [@alebastr](https://github.com/alebastr))
- ~Allow accessing the underlying layer shell surface~ (EDIT: this change was accidentally omitted from the release)
- Bump explicitly supported GTK version to 3.24.22

## [0.2.0] - 28 Jul 2020
- __RELICENSE TO LGPL v3__ (EDIT: we are using LGPL version 3 or any later version)
- Fix long standing issue with [popup grabs](https://github.com/wmww/gtk-layer-shell/issues/9)
- Fix potential compatibility issues between GTK versions with gtk-priv headers
- Add GitHub Actions CI (just building and license check for now)
- Add script to check that are files are licensed correctly
- Use layer_surface->set_layer instead of remapping (when available) (thanks [@alebastr](https://github.com/alebastr))

## [0.1.0] - 17 Sep 2019
- Initial release
