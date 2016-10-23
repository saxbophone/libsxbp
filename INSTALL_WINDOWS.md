# Dependencies

## MinGW

- install MinGW
- make sure `C:\MinGW\bin` is added to `PATH`
- install these packages within MinGW:
  - ?
  - ?

## CMake

- install CMake
- recommended to enable option to add it to `PATH`

## libpng

- libpng installer [http://gnuwin32.sourceforge.net/downlinks/libpng.php](http://gnuwin32.sourceforge.net/downlinks/libpng.php).
- when installing, set install location to `C:\MinGW\` to ensure MinGW can access it.

# Building

- run CMake GUI
- select source code directory
- select a different directory to build in (recommended)
- add the following config variables:
  - `CMAKE_INSTALL_PREFIX` with path value `C:\MinGW`
  - `CMAKE_BUILD_TYPE` with string value `Release`
  - `BUILD_SHARED_LIBS` with bool value `ON` (checked)
- Click `Configure`
- If success, then click `Generate`
- Quit CMake GUI
- open command prompt
- change directory to build directory
- run `mingw32-make all`

## Testing

Once built, tests can be run with `sxp_test.exe`

# Installing

> **Note:** Installing requires admin priveleges, so right-click cmd.exe and select `Run as Administrator`

- run `mingw32-make install`
