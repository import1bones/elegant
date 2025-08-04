#!/bin/bash

# autogen.sh - Generate build system for Elegant Library
# Run this script to generate the configure script from configure.ac

set -e

echo "Generating build system for Elegant Library..."

# Check if autotools are available
command -v autoreconf >/dev/null 2>&1 || {
    echo "Error: autoreconf not found. Please install autotools:"
    echo ""
    echo "  Ubuntu/Debian: sudo apt-get install autoconf automake libtool"
    echo "  CentOS/RHEL:   sudo yum install autoconf automake libtool"
    echo "  macOS:         brew install autoconf automake libtool"
    echo ""
    exit 1
}

# Generate the build system
echo "Running autoreconf..."
autoreconf --install --verbose --force

echo ""
echo "Build system generated successfully!"
echo ""
echo "You can now run:"
echo "  ./configure --enable-debug --enable-examples"
echo "  make"
echo "  make check"
echo ""
