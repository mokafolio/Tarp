find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_executable(HelloWorld HelloWorld.c GL/gl3w.c)
set_target_properties(HelloWorld PROPERTIES COMPILE_FLAGS "-std=c89 -pedantic")
target_link_libraries(HelloWorld ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})

add_executable(Playground Playground.c GL/gl3w.c)
target_link_libraries(Playground ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})
