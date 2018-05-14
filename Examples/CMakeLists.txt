find_package(PkgConfig REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories (${CMAKE_CURRENT_SOURCE_DIR})

add_executable(HelloWorld HelloWorld.c GL/gl3w.c)
target_link_libraries(HelloWorld ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})

#set(CMAKE_C_FLAGS "") 
#add_executable(BasicExample BasicExample.c GL/gl3w.c)
#target_link_libraries(BasicExample ${TARPDEPS} ${GLFW_STATIC_LIBRARIES})
