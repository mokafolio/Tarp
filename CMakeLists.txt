cmake_minimum_required (VERSION 2.8.11)

option(AddressSanitizer "AddressSanitizer" OFF)

project (Tarp C)

find_package(OpenGL REQUIRED)

include_directories (${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/ExampleAndTestDeps /usr/local/include)

SET(TARPINC
Tarp/Tarp.h
Tarp/TarpArray.h
)

#this is only for use in the examples/tests etc.
SET(TARPDEPS
${OPENGL_LIBRARIES}
)

#avoid opengl deprecation warnings on mojave +
if (APPLE)
add_compile_options(-DGL_SILENCE_DEPRECATION)
endif()

foreach ( file ${TARPINC} )
    get_filename_component( dir ${file} DIRECTORY )
    install( FILES ${file} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${dir} )
endforeach()

add_subdirectory (Examples)
