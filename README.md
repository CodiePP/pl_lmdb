
# Interfacing to LMDB from Prolog

[![building the package for SWI Prolog](https://github.com/CodiePP/pl_lmdb/actions/workflows/compilation.yml/badge.svg?branch=main)](https://github.com/CodiePP/pl_lmdb/actions/workflows/compilation.yml)

[LMDB](http://www.lmdb.tech/doc/) is a very fast database which maps committed records into memory.

## compilation

```sh
aclocal --force && autoheader --force && autoconf --force
```
then run
```sh
./configure
```
then call `make swi`

## installation

```sh
mkdir -p ${HOME}/lib/sbcl
cp src/lmdb.qlf ~/lib/sbcl/
cp pllmdb-* ~/lib/sbcl/pllmdb
```

add to the search path for SWI Prolog:
```sh
echo ":- assertz(file_search_path(sbcl,'${HOME}/lib/sbcl'))." >> ${HOME}/.config/swi-prolog/init.pl
```

## tests

this test will populate a database with one million of records:
```sh
swipl -l test/t3.pl -g test
```

this test reads them all back and verifies them:
```sh
swipl -l test/t4.pl -g test
```

