# LDDW : LDD for Windows

A ldd(List Dynamic Dependencies) implementation for windows, written in C.

[fork of Sharp0802/lddw](https://github.com/Sharp0802/lddw)

CURRENT STATE OF LDDW: not usable!

## Why fork?

* make CMake compatible, with supported compilers:
  * clang
  * msvc
* make output more parsable
* add filtering into lddw

## SYNOPSIS

<pre>
lddw [<i>option</i>]... <i>file</i>...
</pre>

## DESCRIPTION

`lddw` prints the shard objects (shared libraries) required by each program or shared object specified on the command line.

In the usual case, `lddw` scan the import table section in PE header of file.
And if the import table is detected in PE header,
For each dependency that is in the import table,
`lddw` displays the location of the matching object.

### SECURITY

Unlinke original [`ldd(1)`](https://man7.org/linux/man-pages/man1/ldd.1.html),
`lddw` doesn't attempt to directly execute the program to obtain the dependency information.
Thus, You can use `lddw` on an untrusted executable.

## OPTIONS

* `--output=cmake`

Paths of dlls are sperated by ';'

* `-h`, `--help`

Usage information

* `-V`, `--version`

Print version information and exit
