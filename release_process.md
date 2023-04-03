# GTK Layer Shell release process
1. Make sure you've pulled latest changes from master
1. Create/checkout branch for this series: `git checkout -b release-A.B`/`git checkout release-A.B`
1. Pull GTK and update gtk-priv: `./gtk-priv/scripts/build.py` (see [gtk-priv/README.md](gtk-priv/README.md) for details)
1. Commit any changes to gtk-priv the script may have made
1. Make sure the tests pass: `ninja -C build test`
1. Compile and play around with gtk-layer-demo: `./build/examples/gtk-layer-demo` (catch anything the automatic tests missed)
1. Bump version in [meson.build](meson.build)
1. Update [compatibility.md](compatibility.md)
1. Update [CHANGELOG.md](CHANGELOG.md) ([GitHub compare](https://github.com/wmww/gtk-layer-shell/compare/) is useful here)
1. Commit and push meson, compatibility and changelog changes
1. Tag release: `git tag vA.B.C`
1. Push tag: `git push origin vA.B.C`
1. PR and merge release branch into master
1. Under Releases in the GitHub repo, the tag should have already appeared, click it
1. Click Edit tag
1. Enter release name (version number, no v prefix) and copy in the changelog
1. Publish release
