# GTK compatibility
__NOTE: just because an app crashes with a warning that links to this page does not mean GTK layer shell is at fault. While I have a hard time keeping up with *explicitly* supporting each new version of GTK, GTK hasn't broken us yet since we started. Only downgrade GTK or open a GTK Layer Shell issue if you've investigated the more probable possibility that the app you're running has some other crash.__

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
| __master__ | v3.20.0 - v3.24.26 | v3.24.31 | - |
