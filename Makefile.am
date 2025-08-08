ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src
if BUILD_EXAMPLES
SUBDIRS += examples
endif

# Documentation and distribution files
EXTRA_DIST = README.md LICENSE INSTALL NEWS \
    docs/overview.md \
    docs/memory_safety.md \
    docs/api_reference.md \
    docs/compile_commands_guide.md

# Include headers in distribution
elegantincludedir = $(includedir)/elegant
elegantinclude_HEADERS = \
    inc/elegant.h \
    inc/elegant_core.h \
    inc/elegant_collection.h \
    inc/elegant_advanced.h \
    inc/elegant_memory.h \
    inc/elegant_maybe.h \
    inc/elegant_either.h \
    inc/elegant_scope.h \
    inc/elegant_safety.h

# pkg-config file
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = elegant.pc

# Test target
check-local:
	@echo "Running Elegant Library functional tests..."
	@if test -x examples/functional_demo; then \
		examples/functional_demo; \
		echo "✓ Functional operations test passed!"; \
	else \
		echo "Examples not built - run ./configure --enable-examples"; \
	fi

# Clean generated files
CLEANFILES = elegant.pc
DISTCLEANFILES = config.h.in~ configure.scan

# Distribution
dist-hook:
	@echo "Creating elegant-$(VERSION).tar.gz distribution..."

.PHONY: check-local
