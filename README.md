# Dusk

## Dusk programming langauge

**Welcome to Dusk!**

Dusk is a simple, compiled programming language with modern syntax which is inspired by programming languages
like [Swift](https://swift.org), [Rust](https://www.rust-lang.org) and [Kotlin](https://kotlinlang.org).

Dusk was developed as a semestral assignemnet for BI-PJP course on Faculty of Infomatics at [CTU](https://www.cvut.cz/en)
and is not intended to have long-time support.

### Build requirements

Dusk can be built on any platform that supports C++17, LLVM 6.0 and CMake 3.4.

[Homebrew](https://brew.sh)
```sh
brew install cmake llvm
```

**Linux**
```sh
sudo apt-get install git cmake clang clang-dev python pkg-config
```

### Getting Sources

Easiest and frankly the only way how to get dusk sources is to clone the dusk repo.

**SSH**

```sh
git clone git@gitlab.fit.cvut.cz:mattapet/dusk-lang.git
cd dusk-lang
```

**HTTPS**

```sh
git clone https://gitlab.fit.cvut.cz/mattapet/dusk-lang.git
cd dusk-lang
```

### Building Dusk

Dusk uses [CMake](https://cmake.org) build tool. To properly build dusk, you need to have CMake 3.4
or highet installed and properly set up `LLVM_CONFIG` enviromental variable. It should be set to the
root directory of LLVM CMake library.

Dusk's default build target is a library for working with Dusk source files. Besides the library
the dusk project also provides a compiler `duskc` and a formatter of dusk code `dusk-format`.
Sources for these executables may be found in `tools` directory.

The build results may be found in `bin` directory.

### Examples

To try Dusk in action check out `examples` folder containing a few really simple programs written in
dusk. Most of them have sematic equivalent in Pascal commented at the bottom of the file.

### References

Project icon - sunset by b a r z i n from the Noun Project
