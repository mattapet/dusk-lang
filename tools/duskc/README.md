# `duskc`

## Dusk compiler

`duskc` is a command line application that can parse and compile Dusk source files.

### Requirements

`duskc` requires a version of `clang` to be installed as well as Dusk standart library `libstddusk`.
It is required to set `DUSK_STDLIB_PATH` to path of where the `libstddusk` is installed. This path
must be the same as `DYLD_LIBRARY_PATH`. Sources of standart library can be found in `dusk-lang/stdlib`
folder.

### Usage

`duskc` always takes a single Dusk source as an argument. All other options are purely optional.
By default, the source file is compiled and linked with the dusk standart library. To learn more
about the usage and the options, run `duskc -help` command.

### Example

This command compiles, properly links a Dusk example file named `gcd.dusk` and produces executable
named `gcd`.

```sh
duskc examples/gcd.dusk -o gcd
```
