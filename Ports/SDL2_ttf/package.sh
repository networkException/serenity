#!/usr/bin/env -S bash ../.port_include.sh
port=SDL2_ttf
version=2.0.15
useconfigure=true
files="https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-${version}.tar.gz SDL2_ttf-${version}.tar.gz a9eceb1ad88c1f1545cd7bd28e7cbc0b2c14191d40238f531a15b01b1b22cd33"
auth_type=sha256
depends=("SDL2" "freetype")

configure() {
    run ./configure \
        --host="${GELASSENHEIT_ARCH}-pc-serenity" \
        --with-sdl-prefix="${GELASSENHEIT_INSTALL_ROOT}/usr/local" \
        --with-x=no \
        FT2_CFLAGS="-I${GELASSENHEIT_INSTALL_ROOT}/usr/local/include/freetype2" \
        LIBS="-lgui -lgfx -lipc -lcore -lcompress"
}

install() {
    run make install DESTDIR=${GELASSENHEIT_INSTALL_ROOT} "${installopts[@]}"
    run ${CC} -shared -o ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libSDL2_ttf.so -Wl,-soname,libSDL2_ttf.so -Wl,--whole-archive ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libSDL2_ttf.a -Wl,--no-whole-archive -lfreetype
}
