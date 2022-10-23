# GTK compatibility
__NOTE: the warning that linked you to this page is probably a red herring. GTK Layer Shell has a hard time *explicitly* supporting each version of GTK, but new unsupported versions almost always work in practice.__

__NOTE: in versions of GTK Layer Shell >= 0.8.0 this warning has been turned off by default. It can be re-enabled by setting `GTK_LAYER_SHELL_UNSUPPORTED_GTK_WARNING=1`__

__Before opening an issue on this repo about an app crashing__
- assume the app is crashing for some other reason, investigate
- if you're at wits end, clone GTK Layer Shell
- check out the tag for the version you were using (ex `git checkout v0.6.0`)
- build
- run `./build/examples/gtk-layer-demo`
- if that also crashes, open an issue

Due to limitations of GTK, GTK Layer Shell depends on unstable GTK internals. Each release works with all versions of GTK >=3.22.0 at the time of release, but future GTK updates may occasionally break them. See [gtk-priv/README.md](gtk-priv/README.md) for more details.

The table below shows which GTK Layer Shell versions work with which GTK versions. If there is a mistake, missing info or you have any other questions, please [open an issue](https://github.com/wmww/gtk-layer-shell/issues).

| GTK Layer Shell | Explicitly supported GTK | Highest GTK known to work | GTK versions known to crash |
|---|---|---|---|
| __v0.1.0__ | v3.20.0 - v3.24.11 | v3.24.24 | - |
| __v0.2.0__ | v3.20.0 - v3.24.21 | v3.24.24 | - |
| __v0.3.0__ | v3.20.0 - v3.24.22 | v3.24.24 | - |
| __v0.4.0__ | v3.20.0 - v3.24.23 | v3.24.24 | - |
| __v0.5.0__ | v3.20.0 - v3.24.23 | v3.24.24 | - |
| __v0.5.1__ | v3.20.0 - v3.24.23 | v3.24.24 | - |
| __v0.5.2__ | v3.20.0 - v3.24.24 | v3.24.26 | - |
| __v0.6.0__ | v3.20.0 - v3.24.26 | v3.24.31 | - |
| __v0.7.0__ | v3.20.0 - v3.24.31 | v3.24.34 | - |
| __v0.8.0__ | v3.20.0 - v3.24.34 | v3.24.34 | - |
| __master__ | v3.20.0 - v3.24.34 | v3.24.34 | - |
