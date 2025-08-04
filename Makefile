# Elegant Functional Programming Library
# Makefile for version 0.0.1

CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Wpedantic -O2 -g
CFLAGS += -Iinc/
LDFLAGS = -lm

# Directories
SRCDIR = src
INCDIR = inc
EXAMPLEDIR = examples
BUILDDIR = build
LIBDIR = lib

# Source files
SOURCES = $(SRCDIR)/elegant.c
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Library
LIBRARY = $(LIBDIR)/libelegant.a
SHARED_LIB = $(LIBDIR)/libelegant.so

# Examples
EXAMPLE_SOURCES = $(wildcard $(EXAMPLEDIR)/*.c)
EXAMPLES = $(EXAMPLE_SOURCES:$(EXAMPLEDIR)/%.c=$(BUILDDIR)/%)

# Targets
.PHONY: all clean library examples install test

all: library examples

# Create directories
$(BUILDDIR):
	mkdir -p $(BUILDDIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

# Build library
library: $(LIBRARY) $(SHARED_LIB)

$(LIBRARY): $(OBJECTS) | $(LIBDIR)
	ar rcs $@ $^
	ranlib $@

$(SHARED_LIB): $(OBJECTS) | $(LIBDIR)
	$(CC) -shared -o $@ $^

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# Build examples
examples: $(EXAMPLES)

$(BUILDDIR)/%: $(EXAMPLEDIR)/%.c $(LIBRARY) | $(BUILDDIR)
	$(CC) $(CFLAGS) $< -L$(LIBDIR) -lelegant $(LDFLAGS) -o $@

# Debug build
debug: CFLAGS += -DELEGANT_DEBUG_MEMORY -DDEBUG -O0
debug: library examples

# Install library (requires sudo)
install: library
	sudo mkdir -p /usr/local/include/elegant
	sudo cp $(INCDIR)/*.h /usr/local/include/elegant/
	sudo cp $(LIBRARY) /usr/local/lib/
	sudo cp $(SHARED_LIB) /usr/local/lib/
	sudo ldconfig

# Test the examples
test: $(BUILDDIR)/functional_demo
	@echo "Running Elegant Library v0.0.1 functional test..."
	./$(BUILDDIR)/functional_demo
	@echo "✓ Functional operations test passed!"

# Clean build files
clean:
	rm -rf $(BUILDDIR) $(LIBDIR)

# Show help
help:
	@echo "Elegant Library Build System"
	@echo "============================"
	@echo ""
	@echo "Targets:"
	@echo "  all       - Build library and examples (default)"
	@echo "  library   - Build static and shared libraries"
	@echo "  examples  - Build example programs"
	@echo "  debug     - Build with debug symbols and memory tracking"
	@echo "  test      - Run example programs"
	@echo "  install   - Install library system-wide (requires sudo)"
	@echo "  clean     - Remove build files"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Build flags:"
	@echo "  CC=$(CC)"
	@echo "  CFLAGS=$(CFLAGS)"
	@echo "  LDFLAGS=$(LDFLAGS)"

# Development shortcuts
dev: debug test

# Package for distribution
dist: clean
	tar czf elegant-0.0.1.tar.gz inc/ src/ examples/ Makefile README.md LICENSE docs/

.SUFFIXES:
.SUFFIXES: .c .o
