#!/usr/bin/env -S bash ../.port_include.sh

port=SDL2_gfx
version=1.0.4
files="https://downloads.sourceforge.net/project/sdl2gfx/SDL2_gfx-${version}.tar.gz SDL2_gfx-${version}.tar.gz 63e0e01addedc9df2f85b93a248f06e8a04affa014a835c2ea34bfe34e576262"
auth_type=sha256
depends=("SDL2")
useconfigure=true
configopts=("--with-sdl-prefix=${GELASSENHEIT_INSTALL_ROOT}/usr/local")

install() {
    run make install DESTDIR=${GELASSENHEIT_INSTALL_ROOT} "${installopts[@]}"
    run ${CC} -shared -o ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libSDL2_gfx.so -Wl,-soname,libSDL2_gfx.so -Wl,--whole-archive ${GELASSENHEIT_INSTALL_ROOT}/usr/local/lib/libSDL2_gfx.a -Wl,--no-whole-archive
}
