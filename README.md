# Saxbospiral ![saxbospiral](saxbospiral.png "saxbospiral")

Experimental generation of 2D spiralling lines based on input binary data

## Dependencies

You will need:

- A compiler that can compile C99 code
- [Cmake](https://cmake.org/) - v3.0 or newer
- [libpng](http://www.libpng.org/pub/png/libpng.html) - (this often comes preinstalled with many modern unix-like systems)

### Note:

These commands are for unix-like systems, without an IDE or other build system besides CMake. If building for a different system, or within an IDE or other environment, consult your IDE/System documentation on how to build CMake projects.

## Build

```sh
cmake .
make
```

## Test

```sh
make test
```

## Install Library + Binaries

This command might require `sudo`, but check your system configuration. For example, it installs to `/usr/local/` by default, which is user-writable on OSX if you use Homebrew, so not requiring admin privileges.

```
make install
```
