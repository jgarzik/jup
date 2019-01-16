# Jup

JSON swiss army knife.

## Introduction

This is a command line utility that may be used to manipulate
JSON documents, adding and deleting many levels deep.

## Dependencies

### univalue

Either install `libunivalue-dev` on Ubuntu, or download and compile
the source code from https://github.com/jgarzik/univalue

## Installation

This project is a standard GNU
[autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)
project.  Build and install instructions are available in the `INSTALL`
file provided with GNU autotools.

```
$ ./autogen.sh
$ ./configure
$ make
$ sudo make install
```

