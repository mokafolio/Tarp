project('Tarp', 'c', default_options : ['c_std=c99'])

if meson.is_subproject() == false
    #remove this rather bogus warning
    add_global_arguments('-Wno-missing-braces', language: 'c')
endif

incDirs = include_directories('.', 'ExampleAndTestDeps')

install_headers(['Tarp/Tarp.h', 'Tarp/TarpArray.h'], subdir: 'Tarp')

tarpDep = declare_dependency(
    include_directories: incDirs
)

if meson.is_subproject() == false
    subdir('Examples')
endif
