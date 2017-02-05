# libsxbp ![libsxbp](libsxbp.png "libsxbp")

[![Build Status](https://travis-ci.org/saxbophone/libsxbp.svg?branch=develop)](https://travis-ci.org/saxbophone/libsxbp) [![License: MPL 2.0](https://img.shields.io/badge/License-MPL%202.0-brightgreen.svg)](https://opensource.org/licenses/MPL-2.0) [![Documentation: http://saxbophone.github.io/libsxbp/](https://img.shields.io/badge/Documentation-Doxygen-2C4AA8.svg)](http://saxbophone.github.io/libsxbp/)

Experimental generation of 2D spiralling lines based on input binary data.

This a C library implementing an experimental idea I had for generating procedural shapes. The library takes input as sequences of bytes and turns the 1s and 0s into a kind of *right-angled spiral*, with the changes in direction of the line encoding the binary data in a lossless manner.

Following Linus Torvalds' example, I have egotistically named this invention the **_saxbospiral_**, after my own online handle. At the time of writing, I couldn't think of a better name for it.

For example, the input text **`cabbages`**, encoded as ASCII gives us the following byte values:

**`0x63 0x61 0x62 0x62 0x61 0x67 0x65 0x73`**

When this is given as input data to the algorithm, the output is the shape shown below:

!['cabbages', shown as a saxbospiral figure from encoded ASCII](example_01_cabbages.png "cabbages', shown as a saxbospiral figure from encoded ASCII")

The algorithm is not limited to text however - any form of input binary data will work to produce a resulting figure, although the length of input data currently is a limiting factor in getting speedy results, if perfection is desired.

Although this project doesn't work in sprints, there is a [*sprint board*](https://github.com/saxbophone/libsxbp/projects/1), which is used for organising issues.

## Please Note

- This is a library only. If you're looking for something that is immediately usable for the end-user, you probably want to look at [sxbp](https://github.com/saxbophone/sxbp) instead, which is a command-line program I wrote which uses libsxbp to render input binary files to PNG images.

- As libsxbp is currently at major version 0, expect the library API to be unstable. I will endeavour as much as possible to make sure breaking changes increment the minor version number whilst in the version 0.x.x series and bugfixes increment the patch version number, but no heavy reliance should be placed on this.

## Licensing

As of **v0.25.0**, libsxbp is licensed under the **Mozilla Public License Version 2.0** (`MPL-2.0`).

This is a permissive software license which puts copyleft protections only on the original source files of the project themselves, but does not extend this copyleft to other sources which use the library. In other words, the library may be used and integrated into other projects regardless of whether they are open-source or commercial, but any modifications to the library itself must be made available under the same or an equivalent license (see license for further details on the nuances of this).

A full verbatim of this license may be found in the [LICENSE](LICENSE) file in this repository. If for some reason you have not received this file, you can view a copy of the license at [https://www.mozilla.org/MPL/2.0/](https://www.mozilla.org/MPL/2.0/).

## Building

Libsxbp can be built without installing for test purposes and for general usage, however if you plan on writing programs that use it I recommend you install it so it will be in your system's standard library and header include locations.

### Dependencies

There are no compulsory library dependencies beyond the C99 standard library. *Yay!*

#### Build Tools

- A compiler that can compile ISO C99 or C11 code
- [Cmake](https://cmake.org/) - v3.0 or newer

#### Optional Libraries

*If you also want to be able to produce images in PNG format with the library, you will need:*
- [libpng](http://www.libpng.org/pub/png/libpng.html) - (this often comes pre-installed with many modern unix-like systems)

> ### Note:

> These commands are for unix-like systems, without an IDE or other build system besides CMake. If building for a different system, or within an IDE or other environment, consult your IDE/System documentation on how to build CMake projects.

> Additionally, it is of worth noting that this library has only been thoroughly tested and developed on **Ubuntu GNU/Linux** with **GCC v5.4.0** and **Clang 3.8.0**. Although every effort has been made to make it as cross-platform as possible (including quite strict **ISO C99** and **ISO C11** compliance), **Your Mileage May Vary**. Bug Reports and Patches for problems running on other systems, particularly **Microsoft Windows** and **Mac OSX** are most welcome.

### Recommended Library Build

Invoke CMake within the root of this repository, with these arguments to make CMake build the library in release mode (with full optimisation) and as a shared library:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .
make
```

The above builds in C99 mode by default. The standard to use can be controlled by the `LIBSXBP_C_STANDARD` CMake variable or by the `LIBSXBP_C_STANDARD` environment variable.

You can build in C11 mode if you want with either of the following:

```sh
# using CMake variable
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON -DLIBSXBP_C_STANDARD=11 .
```

```sh
# using environment variable
LIBSXBP_C_STANDARD=11 cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .
```

Also, by default the CMake build script will look for libpng. If it cannot find it then it will disable support for PNG output.

You may choose to explicitly disable or enable PNG support with the `LIBSXBP_PNG_SUPPORT` CMake variable, which can be passed on the command-line like so:

```sh
# PNG support is required, build will fail if libpng can't be found
cmake -DLIBSXBP_PNG_SUPPORT=ON .
```

```sh
# PNG support is not included, even if libpng can be found
cmake -DLIBSXBP_PNG_SUPPORT=OFF .
```

> ### Note:

> Building as a shared library is recommended as then binaries compiled from [sxbp](https://github.com/saxbophone/sxbp) or your own programs that are linked against the shared version can immediately use any installed upgraded versions of libsxbp with compatible ABIs without needing re-compiling.

## Test

Building the library with the default Make target shown above also compiles the unit tests for libsxbp, to an executable in the same directory. You can either run this directly, use `make test` or use **ctest** (comes bundled with CMake) to run these unit tests (recommended):

```sh
ctest -V  # get verbose output from test running
```

> ### Note:

> It's recommended that if testing for development purposes (rather than just verification that all is working), you run CMake in `Debug` mode instead. This will pass more strict options to your compiler if it supports them (GCC and Clang do), leading to a higher chance of bugs being caught before committal.

## Install Library

Use the `make install` target to install the compiled library and the necessary header files to your system's standard location for these files.

```
make install
```

> ### Note:

> This may or may not require admin privileges, depending on where the files are installed to. Typically does on GNU/Linux, typically doesn't on Mac OSX *with Homebrew installed* and *may or may not* on Microsoft Windows. The CMake variable `CMAKE_INSTALL_PREFIX` may be used to change this destination.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for information related to making contributions to libsxbp.
