# CMakeList.txt : CMake project for nihil-render-cmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("nihil-render")

file(GLOB_RECURSE SOURCES 
	"Classes/*.cpp"
)

# Add source to this project's executable.
add_library (nihil-render STATIC ${SOURCES})

target_include_directories(nihil-render 
	PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/includes
	${CMAKE_CURRENT_SOURCE_DIR}
)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/lib)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/nstd)

target_link_libraries(nihil-render PUBLIC
	nstd
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET nihil-render PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
