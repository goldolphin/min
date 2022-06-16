# min

A simple bytecode instruction set and its embeddable VM written in modern C++.

## Features
1. Builtin modules and procedures
2. Support compound types and runtime type info
3. Accurate garbage collection
4. Native extension

## Build and Play
### Dependencies
1. gcc / clang
2. cmake

### Build
```shell
$ cd /path/to/min/source
$ mkdir debug
$ cd debug
$ cmake ..
$ make -j
```
### Play
```shell
$ ./minas run < example/min/hello.minas
```
