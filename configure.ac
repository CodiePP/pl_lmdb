#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# 1. run 'aclocal'
# 2. run 'autoheader'
# 3. run 'autoconf'
# 4. run './configure'  (optionally with parameters)
# 5. run 'make swi'
#
# subsequently: 'autoreconf -vfi'

AC_PREREQ([2.69])
AC_INIT(pllmdb, 1.0.0)

AC_CONFIG_AUX_DIR([config])
#AC_CONFIG_SRCDIR([test/test.cpp])
AC_CONFIG_HEADERS([src/config.h])

# preset definitions
if test -z "$CFLAGS"; then
  CFLAGS="-g -O2 -fPIC -Wall"
fi

ARCH=$(uname -s)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for libraries.
AC_SUBST(ARCH)
AC_SUBST(SWI_CFLAGS)
AC_SUBST(SWI_LIBS)
AC_SUBST(SWI_BASE)
AC_SUBST(SWI_ARCH)
AC_SUBST(SWI_SOLIBDIR)
AC_SUBST(SWI_PLLIBDIR)
AC_SUBST(LMDB_CFLAGS)
AC_SUBST(LMDB_LIBS)
AC_SUBST(SOTARGET)
AC_SUBST(PLTARGET)

PKG_PROG_PKG_CONFIG()

if [[ $ARCH = "Darwin" ]]; then
  # you likely need to adjust these paths:
  export PKG_CONFIG_PATH="${PKG_CONFIG_PATH}:${HOME}/.local/share/pkgconfig"
fi

PKG_CHECK_MODULES([SWI], [swipl])
if [[ $ARCH != "Darwin" ]]; then
  PKG_CHECK_MODULES([LMDB], [lmdb])
fi

SWI_BASE=$($PKG_CONFIG --variable=PLBASE swipl)
SWI_ARCH=$($PKG_CONFIG --variable=PLARCH swipl)
SWI_SOLIBDIR=$SWI_BASE/lib/$SWI_ARCH
SWI_PLLIBDIR=$SWI_BASE/library

if [[ $ARCH = "Darwin" ]]; then
  for D in /opt/homebrew/opt/lmdb /usr/local/opt/lmdb; do
    if test -d "${D}"; then
      LIBLMDB_BASE=$D
      MACHINE=$(uname -m)
      LMDB_CFLAGS="-I${LIBLMDB_BASE}/include"
      LMDB_LIBS="-L${LIBLMDB_BASE}/lib -llmdb"
    fi
  done
  if [[ -z "$LIBLMDB_BASE" ]]; then AC_MSG_FAILURE([cannot find LMDB]); fi
fi

# Checks for header files.
AC_CHECK_HEADERS([stdint.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_CHECK_SIZEOF(long)

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([strdup])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

