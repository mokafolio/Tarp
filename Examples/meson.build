glfw = dependency('glfw3')
gl = dependency('GL')

deps = [glfw, gl]

#on osx we manually need to link glfw3 dependencies...hmm :/
if host_machine.system() == 'darwin'
    deps += [dependency('Cocoa'), dependency('IOKit'), dependency('CoreFoundation'), dependency('CoreVideo')]
endif

gl3w = '../ExampleAndTestDeps/GL/gl3w.c'

if get_option('addressSanitizer') == true
    add_project_arguments('-fsanitize=address', language:'c')
    add_project_link_arguments('-fsanitize=address', language:'c')
endif

helloWorld = executable('HelloWorld', ['HelloWorld.c', gl3w], dependencies: deps, 
    include_directories: incDirs, c_args: ['-std=c89', '-pedantic'])
renderCacheExample = executable('RenderCacheExample', ['RenderCacheExample.c', gl3w], dependencies: deps, 
    include_directories: incDirs, c_args: ['-std=c89', '-pedantic'])
playground = executable('Playground', ['Playground.c', gl3w], dependencies: deps, 
    include_directories: incDirs)
nestedClipping = executable('NestedClipping', ['NestedClipping.c', gl3w], dependencies: deps, 
    include_directories: incDirs)
