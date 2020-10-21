# GTK compatibility
Due to limitations of GTK, GTK Layer Shell depends on unstable GTK internals. Each release works with all versions of GTK >=3.22.0 at the time of release, but future GTK updates may occasionally break them. See [gtk-priv/README.md](gtk-priv/README.md) for more details.

The table below shows which GTK Layer Shell versions work with which GTK versions. If GTK is between the explicitly supported version and the highest version known to work, the warning that likely sent you to this page is harmless. If there is a mistake, missing info or you have any other questions, please [open an issue](https://github.com/wmww/gtk-layer-shell/issues).

| GTK Layer Shell | Explicitly supported GTK | Highest GTK known to work | GTK versions known to crash |
|---|---|---|---|
| __v0.1.0__ | GTK v3.24.11 | GTK v3.24.23 | - |
| __v0.2.0__ | GTK v3.24.21 | GTK v3.24.23 | - |
| __v0.3.0__ | GTK v3.24.22 | GTK v3.24.23 | - |
| __master__ | GTK v3.24.23 | GTK v3.24.23 | - |
