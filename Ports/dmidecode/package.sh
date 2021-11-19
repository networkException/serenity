#!/usr/bin/env -S bash ../.port_include.sh
port=dmidecode
version=3.3
useconfigure=false
files="https://download.savannah.gnu.org/releases/dmidecode/dmidecode-${version}.tar.xz dmidecode-${version}.tar.xz
https://download.savannah.gnu.org/releases/dmidecode/dmidecode-${version}.tar.xz.sig dmidecode-${version}.tar.xz.sig
https://ftpmirror.gnu.org/gnu/gnu-keyring.gpg gnu-keyring.gpg"
auth_type="sig"
auth_import_key="90DFD6523C57373D81F63D19865688D038F02FC8"
auth_opts=("--keyring" "./gnu-keyring.gpg" "dmidecode-${version}.tar.xz.sig")

install() {
    run make clean
    run make
}

post_install() {
    mkdir -p "${GELASSENHEIT_INSTALL_ROOT}/bin"
    run make install-bin DESTDIR="${GELASSENHEIT_INSTALL_ROOT}"
    ln -sf /usr/local/sbin/dmidecode "${GELASSENHEIT_INSTALL_ROOT}/bin/dmidecode"
    ln -sf /usr/local/sbin/biosdecode "${GELASSENHEIT_INSTALL_ROOT}/bin/biosdecode"
    ln -sf /usr/local/sbin/vpddecode "${GELASSENHEIT_INSTALL_ROOT}/bin/vpddecode"
}
