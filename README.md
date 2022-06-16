# min

A simple bytecode instruction set and its embeddable VM written in modern C++(17).

## Features
1. Builtin modules and procedures
2. Support compound types and runtime type info
3. Accurate garbage collection
4. Native extension

## Instruction Set
* [OP Codes](src/main/min/def/op.def)
* [Directives](src/main/min/def/directive.def)
* [Types](src/main/min/def/types.def)


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
* Create and traverse a singly linked list
```shell
$ ./minas run < example/min/test_list.minas
```
* Test gc
```shell
$ ./minas run < example/min/test_gc.minas
```
