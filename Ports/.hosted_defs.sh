#!/usr/bin/env bash

export GELASSENHEIT_SOURCE_DIR="$(realpath "${SCRIPT}/../")"

if [ "$GELASSENHEIT_TOOLCHAIN" = "Clang" ]; then
    export GELASSENHEIT_BUILD_DIR="${GELASSENHEIT_SOURCE_DIR}/Build/${GELASSENHEIT_ARCH}clang"
    export CC="clang --target=${GELASSENHEIT_ARCH}-pc-serenity --sysroot=${GELASSENHEIT_BUILD_DIR}/Root"
    export CXX="clang++ --target=${GELASSENHEIT_ARCH}-pc-serenity --sysroot=${GELASSENHEIT_BUILD_DIR}/Root"
    export AR="llvm-ar"
    export RANLIB="llvm-ranlib"
    export PATH="${GELASSENHEIT_SOURCE_DIR}/Toolchain/Local/clang/bin:${HOST_PATH}"
else
    export GELASSENHEIT_BUILD_DIR="${GELASSENHEIT_SOURCE_DIR}/Build/${GELASSENHEIT_ARCH}"
    export CC="${GELASSENHEIT_ARCH}-pc-serenity-gcc"
    export CXX="${GELASSENHEIT_ARCH}-pc-serenity-g++"
    export AR="${GELASSENHEIT_ARCH}-pc-serenity-ar"
    export RANLIB="${GELASSENHEIT_ARCH}-pc-serenity-ranlib"
    export PATH="${GELASSENHEIT_SOURCE_DIR}/Toolchain/Local/${GELASSENHEIT_ARCH}/bin:${HOST_PATH}"
fi

export PKG_CONFIG_DIR=""
export PKG_CONFIG_SYSROOT_DIR="${GELASSENHEIT_BUILD_DIR}/Root"
export PKG_CONFIG_LIBDIR="${PKG_CONFIG_SYSROOT_DIR}/usr/lib/pkgconfig/:${PKG_CONFIG_SYSROOT_DIR}/usr/local/lib/pkgconfig"

enable_ccache

DESTDIR="${GELASSENHEIT_BUILD_DIR}/Root"
export GELASSENHEIT_INSTALL_ROOT="$DESTDIR"
