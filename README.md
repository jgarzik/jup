# Jup

JSON updater -- a command line JSON swiss army knife.

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

## Usage

The basic usage is that of a filter.  The basic sequence is,

1. Provide input via stdin.
2. `jup` reads input into memory.
3. `jup` modifies input via a sequence of edit commands.
4. Provide pretty-printed output to stdout (unless `--min` is specified).

### Edit commands

```
$ ./jup --list-short
[
  "array JSON-PATH",
  "false JSON-PATH",
  "file.base64 JSON-PATH FILE",
  "file.csv JSON-PATH FILE",
  "file.hex JSON-PATH FILE",
  "file.json JSON-PATH FILE",
  "file.text JSON-PATH FILE",
  "get JSON-PATH",
  "int JSON-PATH VALUE",
  "new",
  "newarray",
  "null JSON-PATH",
  "num JSON-PATH VALUE",
  "object JSON-PATH",
  "str JSON-PATH VALUE",
  "true JSON-PATH"
]
```

