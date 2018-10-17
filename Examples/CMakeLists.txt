find_package(glfw3 REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_executable(HelloWorld HelloWorld.c ../ExampleAndTestDeps/GL/gl3w.c)
set_target_properties(HelloWorld PROPERTIES COMPILE_FLAGS "-std=c89 -pedantic")
target_link_libraries(HelloWorld ${TARPDEPS} glfw)

add_executable(Playground Playground.c ../ExampleAndTestDeps/GL/gl3w.c)
target_link_libraries(Playground ${TARPDEPS} glfw)
