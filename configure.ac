#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([topiary], [0.0.1], [phewlett76@gmail.com], [topiary], [https://github.com/eccles/liblnxproc])
#
AC_CONFIG_SRCDIR([include/topiary/topiary.h])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL

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

# Checks for library functions.
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([gettimeofday memset regcomp strchr strdup strtoul])

AC_CONFIG_FILES([Makefile
                 include/Makefile
                 include/topiary/Makefile
                 man/Makefile
                 src/Makefile
                 test/Makefile])
AC_OUTPUT
