find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

add_executable(BasicExample BasicExample.cpp)
target_link_libraries(BasicExample ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})
