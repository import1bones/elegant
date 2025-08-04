# Elegant Functional Programming Library v0.0.1

Elegant is a C library that brings Haskell-like functional programming features to the C language with automatic memory management and type safety.

## Features

- **Haskell-like functional programming macros and idioms**
- **Pure functions and immutability support** 
- **Higher-order functions** (functions as arguments and return values)
- **Function composition, currying, and partial application**
- **Map, filter, reduce, and other collection operations**
- **Automatic memory management** with configurable algorithms
- **Custom struct registration** for type-generic operations
- **Inline lambda expressions** for concise functional code
- **Error handling** with Maybe/Option and Either types
- **Pre-compiler checks** for type safety and memory leak detection
- **Declarative and expressive code style**

## Quick Example

```c
#include <elegant.h>

int main() {
    // Automatic allocation, inline lambda, no manual memory management
    AUTO(numbers, elegant_create_array_int(1, 2, 3, 4, 5));
    
    // Map with inline expression - square each number
    AUTO(squares, MAP_INT(numbers, x * x));
    
    // Filter with inline predicate - keep only even numbers  
    AUTO(evens, FILTER_INT(numbers, x % 2 == 0));
    
    // Reduce operation - sum all numbers
    AUTO(sum, REDUCE_INT(numbers, a + b, 0));
    
    printf("Sum: %d\n", sum);
    
    // Memory automatically freed when variables go out of scope
    return 0;
}
```

## Building and Installation

### Requirements

- C99-compatible compiler (GCC, Clang)
- GNU Autotools (autoconf, automake, libtool)
- make
- Standard math library (-lm)

### Quick Build

```bash
# Configure and build
./configure
make

# Run tests
make check

# Install system-wide (optional)
sudo make install
```

### Configure Options

The build system supports several configuration options:

```bash
# Enable debug mode with memory tracking
./configure --enable-debug

# Build example programs  
./configure --enable-examples

# Both debug and examples
./configure --enable-debug --enable-examples

# Custom installation prefix
./configure --prefix=/usr/local

# Show all available options
./configure --help
```

### Development Build

For development with full debugging and examples:

```bash
./configure --enable-debug --enable-examples
make
make check
```

### Distribution Build

To create a distribution tarball:

```bash
make dist
```

This creates `elegant-0.0.1.tar.gz` with all source files needed for building.

### Installation

#### System-wide Installation

```bash
./configure
make
sudo make install
sudo ldconfig  # Update library cache on Linux
```

This installs:
- Headers to `/usr/local/include/elegant/`
- Libraries to `/usr/local/lib/`
- pkg-config file to `/usr/local/lib/pkgconfig/`

#### Custom Installation Location

```bash
./configure --prefix=$HOME/local
make
make install
```

#### Using Installed Library

After installation, compile your programs with:

```bash
# Using pkg-config (recommended)
gcc myprogram.c $(pkg-config --cflags --libs elegant) -o myprogram

# Or manually
gcc myprogram.c -I/usr/local/include/elegant -lelegant -lm -o myprogram
```

### Build System Details

Elegant uses GNU Autotools for a portable, professional build system:

- **configure.ac**: Main autotools configuration
- **Makefile.am**: Top-level build rules
- **src/Makefile.am**: Library build configuration  
- **examples/Makefile.am**: Examples build configuration
- **elegant.pc.in**: pkg-config template

#### Generated Files (Do Not Edit)

The following files are auto-generated and should not be edited directly:
- `configure` - Configuration script
- `Makefile.in`, `src/Makefile.in`, `examples/Makefile.in` - Makefile templates
- `aclocal.m4` - Autotools macros
- `config.h.in` - Configuration header template
- `m4/` directory - Libtool macro files

#### Source Files (Version Controlled)

These files are maintained in git:
- `configure.ac` - Autotools configuration source
- `Makefile.am` - Build rules source
- `elegant.pc.in` - pkg-config file template
- All `.c` and `.h` source files

### Cross-Platform Support

The autotools build system provides excellent cross-platform support:

- **Linux**: Full support with GCC/Clang
- **macOS**: Native support with Xcode tools
- **Windows**: MSYS2/MinGW-w64 environment
- **FreeBSD/OpenBSD**: Native support

### Build Troubleshooting

#### Missing autotools

```bash
# Ubuntu/Debian
sudo apt-get install autoconf automake libtool

# macOS with Homebrew  
brew install autoconf automake libtool

# CentOS/RHEL
sudo yum install autoconf automake libtool
```

#### Regenerating Build System

If you modify `configure.ac` or `Makefile.am`:

```bash
autoreconf --install --force
./configure [options]
make
```

#### Clean Build

```bash
make distclean  # Remove all generated files
./configure [options]
make
```

## Examples and Testing

### Running Examples

With `--enable-examples`:

```bash
# Functional operations demo
./examples/functional_demo

# Basic operations examples
./examples/basic_examples
```

### Test Suite

```bash
make check
```

Runs the built-in test suite that validates:
- Core functional operations (MAP, FILTER, REDUCE)
- Memory management
- Array operations
- Type safety

## Documentation

Detailed documentation is available in:
- `docs/overview.md` - Comprehensive API documentation
- `examples/` - Example programs
- Header files in `inc/` - Inline API documentation

## Memory Management

Elegant provides automatic memory management with configurable strategies:

1. **Stack Arena** (default) - Fast allocation with scope-based cleanup
2. **Reference Counting** - Shared ownership with automatic cleanup
3. **Manual Mode** - For integration with existing memory management

## Compatibility

- **C Standard**: C99 or later
- **Compilers**: GCC 4.9+, Clang 3.5+, ICC 15.0+
- **Platforms**: Linux, macOS, Windows (MSYS2), *BSD
- **Dependencies**: Standard C library, libm (math library)

## Version Information

- **Version**: 0.0.1
- **Git Tag**: v0.0.1
- **License**: See LICENSE file
- **Build System**: GNU Autotools (autoconf/automake/libtool)