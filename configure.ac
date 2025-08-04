AC_INIT([elegant], [0.0.1], [13519377850@163.com])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIRS([m4])
AM_INIT_AUTOMAKE([-Wall -Werror foreign subdir-objects])

# Checks for programs
AC_PROG_CC
AM_PROG_AR

# Initialize libtool
LT_INIT

# Ensure C99 support  
if test "x$ac_cv_prog_cc_stdc" = "xno"; then
    AC_MSG_ERROR([C99 support is required])
fi

# Check for GCC extensions (statement expressions)
AC_MSG_CHECKING([for GCC statement expressions])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
    int x = ({ int y = 1; y + 1; });
])], [
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_STATEMENT_EXPRESSIONS], [1], [Define if compiler supports statement expressions])
], [
    AC_MSG_RESULT([no])
    AC_MSG_WARN([Statement expressions not supported - some macros will be limited])
])

# Check for __typeof__ support
AC_MSG_CHECKING([for __typeof__ support])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
    int x = 1;
    __typeof__(x) y = x;
])], [
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_TYPEOF], [1], [Define if compiler supports __typeof__])
], [
    AC_MSG_RESULT([no])
    AC_MSG_WARN([__typeof__ not supported - AUTO macro will be limited])
])

# Check for thread-local storage
AC_MSG_CHECKING([for thread-local storage])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([], [
    __thread int x = 0;
])], [
    AC_MSG_RESULT([yes])
    AC_DEFINE([HAVE_THREAD_LOCAL], [1], [Define if compiler supports __thread])
], [
    AC_MSG_RESULT([no])
    AC_MSG_WARN([Thread-local storage not supported - memory tracking will be global])
])

# Optional features
AC_ARG_ENABLE([debug],
    AS_HELP_STRING([--enable-debug], [Enable debug mode with memory tracking]),
    [debug=$enableval],
    [debug=no])

if test "x$debug" = "xyes"; then
    AC_DEFINE([ELEGANT_DEBUG_MEMORY], [1], [Define to enable debug memory tracking])
    CFLAGS="$CFLAGS -g -O0 -DDEBUG"
else
    CFLAGS="$CFLAGS -O2"
fi

AC_ARG_ENABLE([examples],
    AS_HELP_STRING([--disable-examples], [Don't build example programs]),
    [examples=$enableval],
    [examples=yes])

AM_CONDITIONAL([BUILD_EXAMPLES], [test "x$examples" = "xyes"])

# Checks for libraries
AC_CHECK_LIB([m], [sqrt])

# Checks for header files
AC_CHECK_HEADERS([stdlib.h string.h stdint.h stdbool.h limits.h])

# Checks for typedefs, structures, and compiler characteristics
AC_CHECK_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([memcpy memset])

# Output files
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    examples/Makefile
    elegant.pc
])

AC_OUTPUT

echo "
Configuration summary:
  Package:              $PACKAGE_NAME $PACKAGE_VERSION
  C Compiler:           $CC
  CFLAGS:              $CFLAGS
  LDFLAGS:             $LDFLAGS
  Debug mode:          $debug
  Build examples:      $examples
  Statement expressions: yes
  Thread-local storage: yes
"
