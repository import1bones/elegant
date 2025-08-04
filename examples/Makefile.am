if BUILD_EXAMPLES

noinst_PROGRAMS = functional_demo basic_examples

# Common settings for all examples
AM_CPPFLAGS = -I$(top_srcdir)/inc
AM_CFLAGS = -Wall -Wextra -std=c99
LDADD = $(top_builddir)/src/libelegant.la

# Individual example programs
functional_demo_SOURCES = functional_demo.c
basic_examples_SOURCES = basic_examples.c
# simple_examples_SOURCES = simple_examples.c  # TODO: Fix Maybe/Either types
# test_suite_SOURCES = test_suite.c  # TODO: Fix test implementation

endif
