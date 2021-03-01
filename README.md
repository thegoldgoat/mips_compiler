# MIPS COMPILER

This compiler is developed just for fun, and the output is suited for the relative MIPS emulator/interpreter

## Compile

Install dependencies first:

1. [meson](https://mesonbuild.com/)
2. [ninja](https://ninja-build.org/)
3. C++ compiler

```bash

# Setup build directory
meson setup build

# Enter build directory
cd build

# Build
ninja

# (optional) install into /usr/local/bin
sudo ninja install

```

## Usage

Assemble as many assembly files as you want, then link them together into a single executable

Run `assemble` or `link` to see CLI parameters:

Usage: assemble <input_path> <output_path> [--verbose]
Usage: link -o <output_path> <input_file1> <input_file2> [...]

## License

see LICENSE
