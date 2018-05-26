# `dusk-format`

## Dusk formatter

`dusk-format` is a command line application that can parse and format Dusk source files.

### Requirements

There are no requirements as far the enviroments matters. The only necessity is installed `dusk-format` CLI.

### Usage

`dusk-format` always takes a single Dusk source file as an argument. All other options are purely optional.
Indentation with 4 spaces is used by default. Formatted source file is printed to the standart output.
To learn more about the usage and the options, run `dusk-format -help` command.


### Example

This command formats Dusk source file named `gcd.dusk` with 2 space indentation.

```sh
dusk-format examples/gcd.dusk -indent-size=2 -indent-type=IndKind::Space
```
