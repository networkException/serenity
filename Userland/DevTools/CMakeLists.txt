add_subdirectory(Inspector)
add_subdirectory(Playground)
add_subdirectory(Profiler)
add_subdirectory(HackStudio)

if("${GELASSENHEIT_ARCH}" STREQUAL "i686")
	add_subdirectory(UserspaceEmulator)
endif()
