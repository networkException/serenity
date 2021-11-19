#!/usr/bin/env -S bash ../.port_include.sh
port=libogg
version=1.3.4
useconfigure=true
files="https://github.com/xiph/ogg/releases/download/v${version}/libogg-${version}.tar.gz libogg-${version}.tar.gz fe5670640bd49e828d64d2879c31cb4dde9758681bb664f9bdbf159a01b0c76e"
auth_type=sha256

install() {
    run make DESTDIR=${GELASSENHEIT_INSTALL_ROOT} "${installopts[@]}" install
    ${CC} -shared -o ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libogg.so -Wl,-soname,libogg.so -Wl,--whole-archive ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libogg.a -Wl,--no-whole-archive
    rm -f ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libogg.la
}
