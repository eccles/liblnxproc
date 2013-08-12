#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([topiary], [0.0.1], [phewlett76@gmail.com], [topiary], [https://github.com/eccles/liblnxproc])
#
AM_INIT_AUTOMAKE
#
LT_PREREQ([2.2])
LT_INIT
#
AC_CONFIG_SRCDIR([include/topiary/topiary.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

AC_CONFIG_MACRO_DIR([m4])

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h stddef.h stdlib.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_INLINE
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Check for optional programs
# 1. doxygen support

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], 
[test -n "$DOXYGEN"])AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

# 2. gcov support

AC_ARG_ENABLE([gcov],
    AS_HELP_STRING([--enable-gcov],
        [use Gcov to test the test suite (requires --disable-shared)]),
        [enable_gcov="yes"],
        [enable_gcov=no])

if test "x$enable_gcov" = "xyes"; then
    AC_CHECK_PROGS([GCOV], [gcov])
    if test -z "$GCOV"; then
        AC_MSG_WARN([Gcov not found - continuing without Gcov support])
        enable_gcov=no
    fi
fi

if test "x$enable_gcov" = "xyes"; then
    if test "x$enable_shared" = "xyes"; then
        AC_MSG_WARN([Gcov requires disable-shared - continuing without Gcov support])
        enable_gcov=no
    fi
fi

AM_CONDITIONAL([HAVE_GCOV], test "$enable_gcov" = "yes")

# 3. lcov support

AC_ARG_ENABLE([lcov],
    AS_HELP_STRING([--enable-lcov],
        [use Lcov to test the test suite (requires --disable-shared)]),
        [enable_lcov="yes"],
        [enable_lcov=no])

if test "x$enable_lcov" = "xyes"; then
    AC_CHECK_PROGS([LCOV], [lcov])
    if test -z "$LCOV"; then
        AC_MSG_WARN([Lcov not found - continuing without Lcov support])
        enable_lcov=no
    fi
fi

if test "x$enable_lcov" = "xyes"; then
    if test "x$enable_shared" = "xyes"; then
        AC_MSG_WARN([Lcov requires disable-shared - continuing without Lcov support])
        enable_lcov=no
    fi
fi
AM_CONDITIONAL([HAVE_LCOV], test "$enable_lcov" = "yes")

if test "x${enable_gcov}" = "xyes" || test "x${enable_lcov}" = "xyes"; then
    #CFLAGS="${CFLAGS} -DDEBUG=1 -fno-inline --coverage"
    CFLAGS="${CFLAGS} -fno-inline --coverage"
fi

#
# Checks for library functions.
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([gettimeofday memset regcomp strchr strdup strtoul])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 include/Makefile
                 man/Makefile
                 docs/Makefile
                 lib/Makefile
                 bin/Makefile])
AC_OUTPUT

echo \
"-----------------------------------------------------------------------------
${PACKAGE_NAME} Version ${PACKAGE_VERSION}

Prefix: '${prefix}'
Compiler: '${CC} ${CFLAGS}'
Gcov enabled : '${enable_gcov}'
Lcov enabled : '${enable_lcov}'
Enable shared library : '${enable_shared}'

Now type 'make @<:@<target>@:>@'

where the optional <target> is:
    all                 - build libraries,binaries
    install             - install everything

-----------------------------------------------------------------------------"

