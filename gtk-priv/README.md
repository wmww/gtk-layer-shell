# GTK private

This subproject allows access into private and unstable GTK structs. The headers in h are automatically constructed from multiple versions of GTK3. This lets us be compatible with all supported GTK3 versions, despite using internals that are not part of the stable API.

This was originally developed as a standalone project under the name gtk3-espionage. Git history can be found [here](https://github.com/wmww/gtk3-espionage).

## Building the headers

__NOTE: you don't need to rebuild the headers in order to build GTK Layer Shell__. Building the header files is only required when GTK has been updated or a structure is added.

To build, run `./scripts/build.py`

This script will
- Load the list of structures from [scripts/config.py](scripts/config.py)
- Clone the official GTK git repository
- Detect all supported versions of GTK
- Check out and extract data from each version
- Write header files for each structure
