find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_executable(BasicExample BasicExample.c GL/gl3w.c)
target_link_libraries(BasicExample ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})
