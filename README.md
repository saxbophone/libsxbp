# Saxbospiral ![saxbospiral](saxbospiral.png "saxbospiral")

Experimental generation of 2D spiralling lines based on input binary data

## Dependencies

### Library

For the library, you will need:

- A compiler that can compile ISO C99 code
- [Cmake](https://cmake.org/) - v3.0 or newer
- [libpng](http://www.libpng.org/pub/png/libpng.html) - (this often comes preinstalled with many modern unix-like systems)

### CLI

For the included CLI program, you will also need:

- [Argtable 2](http://argtable.sourceforge.net/) - must use v2, v1 and v3 will not work

> ### Note:

> These commands are for unix-like systems, without an IDE or other build system besides CMake. If building for a different system, or within an IDE or other environment, consult your IDE/System documentation on how to build CMake projects.

> Additionally, it is of worth noting that this library has only been thoroughly tested and developed on **Ubuntu GNU/Linux** with **GCC v5.4.0**, although every effort has been made to make it as cross-platform as possible (including reasonably strict **ISO C 99** compliance). It should compile under any POSIX-compliant system with the correct additional dependencies listed. **v0.8** is known to successfully cross-compile from Ubuntu to Windows (via [cygwin](https://www.cygwin.com/)) and Max OSX (using a locally-built compiler toolchain provided via [OSXCross](https://github.com/tpoechtrager/osxcross). It is highly likely that all other versions cross-compile as well (but I haven't yet verified this).

## Basic Build

```sh
cmake .
make
```

## Recommended Library Build

Add two custom options to CMake to build the library in release mode (with full optimisation) and as a shared dynamic library:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .
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
