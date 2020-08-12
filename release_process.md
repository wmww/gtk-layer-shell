# GTK Layer Shell release process
1. Make sure you actually have the latest changes
2. Create/checkout branch for this series: `git checkout -b release-A.B`/`git checkout release-A.B`
3. Pull GTK and update gtk-priv: `./gtk-priv/scripts/build.py` (see [gtk-priv/README.md](gtk-priv/README.md) for details)
4. Commit any changes to gtk-priv the script may have made
5. Compile and play around with gtk-layer-demo (TODO: add more in-depth manual testing process)
6. Bump version in [meson.build](meson.build)
7. Update [CHANGELOG.md](CHANGELOG.md) ([GitHub compare](https://github.com/wmww/gtk-layer-shell/compare/) is useful here)
8. Commit and push version and changelog
9. Tag release: `git tag vA.B.C`
10. Push tag: `git push origin vA.B.C`
11. PR and merge release branch into master
12. Under Releases in the GitHub repo, the tag should have already appeared, click it
13. Click Edit tag
14. Enter release name (version number, no v prefix) and copy in the changelog
15. Publish release
