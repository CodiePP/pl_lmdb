
# Interfacing to LMDB from Prolog

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

## examples


