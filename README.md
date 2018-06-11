# CJello

CJello is a C implementation of the virtual architecture used in my previous Rusty Jello project. To create the memory images used in CJello you can use `rusty_jello FILE -a` to export a memory image from Rusty Jello.

## Installing

Installing should be as simple as:

```
git clone https://github.com/JMAlego/CJello.git
cd CJello
# To compile
make
# To install
make install
```

The make file uses `clang` by default but it should build fine with `gcc`.

While CJello hasn't been tested on Windows, it should work with limited changes as no non-standard libraries are used.

## Running

The current usage string is shown below:

```
Usage: cjello [options] file...
Options:
  -v, --version  Displays a version message
  -h, --help     Displays this help message
  -d, --debug    Enables debug messages
```