project('Tarp', 'c', default_options : ['c_std=c99'])

#remove this rather bogus warning
add_global_arguments('-Wno-missing-braces', language: 'c')

incDirs = include_directories('.', 'ExampleAndTestDeps')

install_headers(['Tarp/Tarp.h', 'Tarp/TarpArray.h'], subdir: 'Tarp')

subdir('Examples')
