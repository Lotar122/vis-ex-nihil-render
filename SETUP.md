# 1 3rd party libraries
you need to add thirdparty/include as external include directories.
add thirdparty/lib as library directory
link against vulkan-1.lib, glfw3_mt.lib

# 2 nihil-standard
you need to add nihil-standard as an include directory, and an external include directory
add nihil-standard as a library directory
link against nstd.lib

# 3
of course link against the nihil-render.lib
and add nihil.hpp as an external include directory