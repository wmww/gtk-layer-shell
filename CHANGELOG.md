# Changelog

## [Unreleased]

## [0.3.0] - 12 Aug 2020
- Demo: Display GTK version
- Demo: Add option for disabling gtk-layer-shell for testing
- Fix intermittent crash relating to popovers ([#51](https://github.com/wmww/gtk-layer-shell/issues/51))
- Clarify that our license is LGPL version 3 or any later version
- Use system XDG Shell protocol when available (thanks [@alebastr](https://github.com/alebastr))
- Allow accessing the underlying layer shell surface
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
