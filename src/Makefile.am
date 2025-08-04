lib_LTLIBRARIES = libelegant.la

libelegant_la_SOURCES = elegant.c

# Include paths
AM_CPPFLAGS = -I$(top_srcdir)/inc

# Compiler flags
AM_CFLAGS = -Wall -Wextra -std=c99

# Library version info (current:revision:age)
# Update when:
# - current: increment when interfaces added/removed/changed
# - revision: increment when implementation changes
# - age: increment when interfaces added, reset when removed
libelegant_la_LDFLAGS = -version-info 0:1:0

# Link with math library if needed
libelegant_la_LIBADD = $(LIBM)
