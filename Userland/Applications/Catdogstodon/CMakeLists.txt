serenity_component(
    Catdogstodon
    REQUIRED
    TARGETS Catdogstodon
)

compile_gml(Post.gml PostGML.h post_gml)

set(SOURCES
    main.cpp
    MainWidget.cpp
    PostWidget.cpp
    PostGML.h
)

serenity_bin(Catdogstodon)
target_link_libraries(Catdogstodon PRIVATE LibCore LibGfx LibGUI LibMain LibProtocol)
