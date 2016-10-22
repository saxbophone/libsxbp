# libsaxbospiral ![libsaxbospiral](libsaxbospiral.png "libsaxbospiral")

![AGPL v3 Logo](agplv3-88x31.png "A logo used to show the AGPL v3.0 License") [![Build Status](https://travis-ci.org/saxbophone/libsaxbospiral.svg?branch=develop)](https://travis-ci.org/saxbophone/libsaxbospiral)

Experimental generation of 2D spiralling lines based on input binary data.

This a C library implementing an experimental idea I had for generating procedural shapes. The library takes input as sequences of bytes and turns the 1s and 0s into a kind of *right-angled spiral*, with the changes in direction of the line encoding the binary data in a lossless manner.

For example, the input text **`cabbages`**, encoded as ASCII gives us the following byte values:

**`0x63 0x61 0x62 0x62 0x61 0x67 0x65 0x73`**

When this is given as input data to the algorithm, the output is the shape shown below:

!['cabbages', shown as a saxbospiral figure from encoded ASCII](example_01_cabbages.png "cabbages', shown as a saxbospiral figure from encoded ASCII")

The algorithm is not limited to text however - any form of input binary data will work to produce a resulting figure, although the length of input data currently is a limiting factor in getting speedy results, if perfection is desired.

## Please Note

- This is a library only. If you're looking for something that is immediately usable for the end-user, you probably want to look at [sxbp](https://github.com/saxbophone/sxbp) instead, which is a command-line program I wrote which uses libsaxbospiral to render input binary files to PNG images.

- As libsaxbospiral is currently at major version 0, expect the library API to be unstable. I will endeavour as much as possible to make sure breaking changes increment the minor version number whilst in the version 0.x.x series and bugfixes increment the patch version number, but no heavy reliance should be placed on this.

## Licensing

Libsaxbospiral is released under version **3.0** of the **GNU Affero General Public License** (AGPLv3).

A full verbatim of this license may be found in the [LICENSE](LICENSE) file in this repository. *You should almost certainly read it*. If for some reason you have not received this file, you can view a copy of the license at [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

Note that as well as being under the same copyleft protections as the GPL License, the AGPL enforces these protections further by **including provision of the software via a network service (such as a website) as one of its definitions of distribution**, hence requiring those who integrate libsaxbospiral into their website or other network service to also release the software into which they are integrating libsaxbospiral under this same license (AGPLv3).

## Building + Installing

Libsaxbospiral can be built without installing for test purposes and for general usage, however if you plan on writing programs that use it I recommend you install it so it will be in your system's standard library and header include locations.

### Dependencies

You will need:

- A compiler that can compile ISO C99 code
- [Cmake](https://cmake.org/) - v3.0 or newer
- [libpng](http://www.libpng.org/pub/png/libpng.html) - (this often comes preinstalled with many modern unix-like systems)

> ### Note:

> These commands are for unix-like systems, without an IDE or other build system besides CMake. If building for a different system, or within an IDE or other environment, consult your IDE/System documentation on how to build CMake projects.

> Additionally, it is of worth noting that this library has only been thoroughly tested and developed on **Ubuntu GNU/Linux** with **GCC v5.4.0** and **Clang 3.8.0**. Although every effort has been made to make it as cross-platform as possible (including quite strict **ISO C 99** compliance), **Your Mileage May Vary**. Bug Reports and Patches for problems running on other systems, particularly **Microsoft Windows** and **Mac OSX** are most welcome.

## Recommended Library Build

Invoke CMake within the root of this repository, with these arguments to make CMake build the library in release mode (with full optimisation) and as a shared library:

```sh
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON .
make
```

> ### Note:

> Building as a shared library is recommended as then binaries compiled from [sxbp](https://github.com/saxbophone/sxbp) or your own programs that are linked against the shared version can immediately use any installed upgraded versions of libsaxbospiral with compatible ABIs without needing re-compiling.

## Test

Building the library with the default Make target shown above also compiles the unit tests for libsaxbospiral, to an executable in the same directory. You can either run this directly, use `make test` or use **ctest** (comes bundled with CMake) to run these unit tests (recommended):

```sh
ctest -V  # get verbose output from test running
```

> ### Note:

> It's recommended that if testing for development purposes (rather than just verification that all is working), you run CMake in `Debug` mode instead. This will pass more strict options to your compiler if it supports them (GCC and Clang do), leading to a higher chance of bugs being caught before commital.

## Install Library

Use the `make install` target to install the compiled library and the neccessary header files to your system's standard location for these files.

```
make install
```

> ### Note:

> This may or may not require admin priveleges. Typically does on GNU/Linux, typically doesn't on Mac OSX *with Homebrew installed* and *probably doesn't* on Microsoft Windows.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for information related to making contributions to libsaxbospiral.
