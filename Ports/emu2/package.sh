#!/usr/bin/env -S bash ../.port_include.sh
port=emu2
version="2021.01"
files="https://github.com/dmsc/emu2/archive/refs/tags/v${version}.tar.gz emu2-${version}.tar.gz 32ea656ad9b034d2c91a20f1a9ac1779cb6905a019c5bdeda9338cfd673bbd72"
auth_type=sha256

build() {
    export CC="${GELASSENHEIT_SOURCE_DIR}/Toolchain/Local/${GELASSENHEIT_ARCH}/bin/${GELASSENHEIT_ARCH}-pc-serenity-gcc"
    run make DESTDIR="${GELASSENHEIT_INSTALL_ROOT}" CC="${CC}" "${installopts[@]}"
}
