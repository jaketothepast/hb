project(HostBlocker)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONC REQUIRED json-c)

add_executable(hb main.c)
target_link_libraries(hb ${JSONC_LIBS})
target_include_directories(hb PUBLIC ${JSONC_INCLUDE_DIRS})