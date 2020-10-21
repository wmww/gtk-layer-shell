# GTK Layer Shell release process
1. Make sure you actually have the latest changes
1. Create/checkout branch for this series: `git checkout -b release-A.B`/`git checkout release-A.B`
1. Pull GTK and update gtk-priv: `./gtk-priv/scripts/build.py` (see [gtk-priv/README.md](gtk-priv/README.md) for details)
1. Commit any changes to gtk-priv the script may have made
1. Compile and play around with gtk-layer-demo (TODO: add more in-depth manual testing process)
1. Bump version in [meson.build](meson.build)
1. Update [CHANGELOG.md](CHANGELOG.md) ([GitHub compare](https://github.com/wmww/gtk-layer-shell/compare/) is useful here)
1. Commit and push version and changelog
1. Tag release: `git tag vA.B.C`
1. Push tag: `git push origin vA.B.C`
1. PR and merge release branch into master
1. Under Releases in the GitHub repo, the tag should have already appeared, click it
1. Click Edit tag
1. Enter release name (version number, no v prefix) and copy in the changelog
1. Publish release
