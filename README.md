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

### Edit commands summary

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

### Full edit command descriptions.

```
$ ./jup --list-commands
[
  {
    "command": "array",
    "usage": "array JSON-PATH",
    "help": "Store empty array at JSON-PATH"
  },
  {
    "command": "false",
    "usage": "false JSON-PATH",
    "help": "Store boolean false at JSON-PATH"
  },
  {
    "command": "file.base64",
    "usage": "file.base64 JSON-PATH FILE",
    "help": "Store (binary?) base64-encoded content of FILE at JSON-PATH"
  },
  {
    "command": "file.csv",
    "usage": "file.csv JSON-PATH FILE",
    "help": "Decode and store CSV-formatted content of FILE at JSON-PATH"
  },
  {
    "command": "file.hex",
    "usage": "file.hex JSON-PATH FILE",
    "help": "Store (binary?) hex-encoded content of FILE at JSON-PATH"
  },
  {
    "command": "file.json",
    "usage": "file.json JSON-PATH FILE",
    "help": "Store content of JSON FILE at JSON-PATH"
  },
  {
    "command": "file.text",
    "usage": "file.text JSON-PATH FILE",
    "help": "Store content of FILE at JSON-PATH"
  },
  {
    "command": "get",
    "usage": "get JSON-PATH",
    "help": "Replace document with subset of JSON input, starting at JSON-PATH"
  },
  {
    "command": "int",
    "usage": "int JSON-PATH VALUE",
    "help": "Store integer VALUE at JSON-PATH"
  },
  {
    "command": "new",
    "usage": "new",
    "help": "Create document with empty object.  stdin ignored."
  },
  {
    "command": "newarray",
    "usage": "newarray",
    "help": "Create document with empty array.  stdin ignored."
  },
  {
    "command": "null",
    "usage": "null JSON-PATH",
    "help": "Store null at JSON-PATH"
  },
  {
    "command": "num",
    "usage": "num JSON-PATH VALUE",
    "help": "Store floating point VALUE at JSON-PATH"
  },
  {
    "command": "object",
    "usage": "object JSON-PATH",
    "help": "Store empty object at JSON-PATH"
  },
  {
    "command": "str",
    "usage": "str JSON-PATH VALUE",
    "help": "Store VALUE at JSON-PATH"
  },
  {
    "command": "true",
    "usage": "true JSON-PATH",
    "help": "Store boolean true at JSON-PATH"
  }
]
```
