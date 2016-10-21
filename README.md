# libsaxbospiral ![libsaxbospiral](libsaxbospiral.png "libsaxbospiral")

Experimental generation of 2D spiralling lines based on input binary data.

This a C library implementing an experimental idea I had for generating procedural shapes. The library takes input as sequences of bytes and turns the 1s and 0s into a kind of *right-angled spiral*, with the changes in direction of the line encoding the binary data in a lossless manner.

For example, the input text **`cabbages`**, encoded as ASCII gives us the following byte values:

**`0x63 0x61 0x62 0x62 0x61 0x67 0x65 0x73`**

When this is given as input data to the algorithm, the output is the shape shown below:

!['cabbages', shown as a saxbospiral figure from encoded ASCII](examples/01_cabbages.png "cabbages', shown as a saxbospiral figure from encoded ASCII")

The algorithm is not limited to text however - any form of input binary data will work to produce a resulting figure, although the length of input data currently is a limiting factor in getting speedy results, if perfection is desired.

## Please Note

- This is a library only. If you're looking for something that is immediately usable for the end-user, you probably want to look at [sxbp](https://github.com/saxbophone/sxbp) instead, which is a command-line program I wrote which uses libsaxbospiral to render input binary files to PNG images.

- As libsaxbospiral is currently at major version 0, expect the library API to be unstable. I will endeavour as much as possible to make sure breaking changes increment the minor version number whilst in the version 0.x.x series and bugfixes increment the patch version number, but no heavy reliance should be placed on this.

## Licensing

Libsaxbospiral is released under version **3.0** of the **GNU Affero General Public License** (AGPLv3).

A full verbatim of this license may be found in the [LICENSE](LICENSE) file in this repository. If for some reason you have not received this file, you can view a copy of the license at [http://www.gnu.org/licenses/](http://www.gnu.org/licenses/).

Note that as well as being under the same copyleft protections as the GPL License, the AGPL enforces these protections further by **including provision of the software via a network service (such as a website) as one of its definitions of distribution**, hence requiring those who integrate libsaxbospiral into their website or other network service to also release the software into which they are integrating libsaxbospiral under this same license (AGPLv3).

## Installing

See [INSTALL.md](INSTALL.md) for instructions on how to build and/or install libsaxbospiral.

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md) for information related to making contributions to libsaxbospiral.
