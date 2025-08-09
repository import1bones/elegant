# Elegant Library - Compiler Requirements

## Supported Compilers

**The Elegant Library requires GCC (GNU Compiler Collection) for full functionality.**

### ✅ Supported

- **GCC** (GNU Compiler Collection) - All versions with C99 support

### ❌ Not Supported

- **Clang** (including Apple Clang on macOS)
- **MSVC** (Microsoft Visual C++)
- **ICC** (Intel C Compiler)
- **Other C compilers**

## Why GCC Only?

The Elegant Library uses **nested functions**, which is a GNU C extension that allows defining functions inside other functions. This feature is essential for the library's functional programming macros like `MAP_INT`, `FILTER_INT`, `REDUCE_INT`, etc.

```c
// Example: This requires nested functions (GCC extension)
#define MAP_INT(arr, expr) ({ \
    int _map_func(int x) { return (expr); } \
    elegant_map_int((arr), _map_func); \
})
```

## How to Use GCC

### Ubuntu/Debian

```bash
sudo apt-get install gcc
make
```

### macOS

```bash
# Install GCC via Homebrew
brew install gcc

# Compile with GCC instead of Clang
CC=gcc-13 make  # Replace 13 with your GCC version
```

### CentOS/RHEL/Fedora

```bash
sudo yum install gcc     # CentOS/RHEL
sudo dnf install gcc     # Fedora
make
```

## Compilation Examples

```bash
# Force use of GCC
CC=gcc make

# Or specify GCC version explicitly
CC=gcc-13 make

# For autotools-based builds
./configure CC=gcc
make
```

## Error Messages

If you try to compile with an unsupported compiler, you'll see:

```bash
error: "ELEGANT LIBRARY COMPILER REQUIREMENT ERROR"
       "This library requires GCC compiler with nested functions support."
       "Currently detected compiler is not supported."
```

This is intentional - the library will not compile with unsupported compilers to avoid runtime issues.

## Alternative Solutions

If you cannot use GCC:

1. **Install GCC** - This is the recommended solution
2. **Use Docker** - Run compilation in a GCC-enabled container
3. **Virtual Machine** - Use a Linux VM with GCC for development
4. **Consider alternatives** - Look for other functional programming libraries for C that don't require GCC extensions

## Technical Details

The library uses these GCC-specific features:

- **Nested functions** - Functions defined inside other functions
- **Statement expressions** - `({ ... })` syntax
- **`__typeof__`** - Type inference (has alternatives but works best with GCC)

These features are not part of the C standard and are specific to GCC.
