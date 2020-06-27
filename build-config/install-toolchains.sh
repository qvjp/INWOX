#! /bin/sh

set -e

TIME_START=`date`
binutils_repo=https://github.com/qvjp/binutils-2.34-inwox.git
gcc_repo=https://github.com/qvjp/gcc-10.1.0-inwox.git

[ -z "${PREFIX+x}" ] && PREFIX="$HOME/inwox-toolchain"
[ -z "$SRCDIR" ] && SRCDIR="$HOME/src"
[ -z "$BUILDDIR" ] && BUILDDIR="$SRCDIR"
[ -z "$ARCH" ] && ARCH=i686
[ -z "$TARGET" ] && TARGET=$ARCH-inwox
[ -z "$SYSROOT" ] && echo "Error: \$SYSROOT not set" && exit 1

SYSROOT="$(cd "$SYSROOT" && pwd)"

CPU_CORE=`cat /proc/cpuinfo | grep "processor" | wc -l`

current_binutils=$(cat "$PREFIX/binutils-commit" || echo Not installed)
current_gcc=$(cat "$PREFIX/gcc-commit" || echo Not installed)
latest_binutils=$(git ls-remote $binutils_repo HEAD | cut -c 1-40)
latest_gcc=$(git ls-remote $gcc_repo HEAD | cut -c 1-40)
echo $current_binutils
echo $current_gcc
echo $latest_binutils
echo $latest_gcc
[ "$current_binutils" = "$latest_binutils" ] && \
        [ "$current_gcc" = "$latest_gcc" ] && \
        echo Cached Toolchain is already up to date. && exit
rm -rf "$PREFIX"

SYSROOT="$(cd "$SYSROOT" && pwd)"

export PATH="$PREFIX/bin:$PATH"

rm -rf "$SRCDIR/inwox-binutils" "$SRCDIR/inwox-gcc"
rm -rf "$BUILDDIR/build-binutils" "$BUILDDIR/build-gcc"

echo Downloading sources...
mkdir -p "$SRCDIR"
cd "$SRCDIR"
git clone $binutils_repo inwox-binutils
git clone $gcc_repo inwox-gcc

echo Building binutils...
mkdir -p "$BUILDDIR/build-binutils"
cd "$BUILDDIR/build-binutils"
"$SRCDIR/inwox-binutils/configure" --target=$TARGET --prefix="$PREFIX" --with-sysroot="$SYSROOT" \
        --disable-nls --disable-werror
make -j"$CPU_CORE"
make install

echo Building gcc...
mkdir -p "$BUILDDIR/build-gcc"
cd "$BUILDDIR/build-gcc"
"$SRCDIR/inwox-gcc/configure" --target=$TARGET --prefix="$PREFIX" --disable-nls \
        --enable-languages=c,c++ --with-sysroot="$SYSROOT"
make -j"$CPU_CORE" all-gcc all-target-libgcc
make install-gcc install-target-libgcc

cd "$SRCDIR/inwox-binutils"
git rev-parse HEAD > "$PREFIX/binutils-commit"
cd "$SRCDIR/inwox-gcc"
git rev-parse HEAD > "$PREFIX/gcc-commit"

TIME_DONE=`date`
echo Install completed
echo "CPU CORE: $CPU_CORE"
echo "Start: $TIME_START"
echo "Done : $TIME_DONE"
