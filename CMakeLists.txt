  cmake_minimum_required (VERSION 3.20)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (
	MYPROJECT
	VERSION 0.0.0
	LANGUAGES C CXX
) 
 
# Directories   

set(SRC_DIR "${PROJECT_SOURCE_DIR}/files/src/") 
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/files/src/") 
set(LIB_DIR "${PROJECT_SOURCE_DIR}/files/libs/") 
 
set(IMGUI_DIR "${PROJECT_SOURCE_DIR}/files/libs/imgui")
set(IMGUI_BACKENDS_DIR "${PROJECT_SOURCE_DIR}/files/libs/imgui/backends")
set(IMGUI_MISC_DIR "${PROJECT_SOURCE_DIR}/files/libs/imgui/misc")

set(IMPLOT_DIR "${PROJECT_SOURCE_DIR}/files/libs/implot")    
  
# Fetch files   
 
file(GLOB_RECURSE CPP_FILES CMAKE_CONFIGURE_DEPENDS 
	"${SRC_DIR}/*.cpp"
	"${SRC_DIR}/*.c"  
) 
 
file(GLOB_RECURSE HPP_FILES CMAKE_CONFIGURE_DEPENDS  
	"${INCLUDE_DIR}/*.hpp"
	"${INCLUDE_DIR}/*.h"
) 

file(GLOB_RECURSE LIB_FILES CMAKE_CONFIGURE_DEPENDS
	"${LIB_DIR}/*.h"
	"${LIB_DIR}/*.c"
)  

file(GLOB IMGUI_FILES CMAKE_CONFIGURE_DEPENDS
	"${IMGUI_DIR}/*.cpp"
	"${IMGUI_DIR}/*.h"  
	"${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp"
	"${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.h"
	"${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp"
	"${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.h"
	"${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3_loader.h"
	"${IMGUI_MISC_DIR}/imgui_stdlib.cpp"
)  

file(GLOB IMPLOT_FILES CMAKE_CONFIGURE_DEPENDS
	"${IMPLOT_DIR}/*.cpp"
	"${IMPLOT_DIR}/*.h"  
	"${IMPLOT_DIR}/implot.h" 
	"${IMPLOT_DIR}/implot_internal.h"
	"${IMPLOT_DIR}/implot.cpp"
	"${IMPLOT_DIR}/implot_items.cpp"
)  
 
# Create Project     

add_executable( MYPROJECT
	${CPP_FILES}
	${HPP_FILES}
	${LIB_FILES}
	${IMGUI_FILES}
	${IMPLOT_FILES}
)  

target_include_directories(MYPROJECT PUBLIC ${INCLUDE_DIR})
target_include_directories(MYPROJECT PUBLIC ${LIB_DIR})
target_include_directories(MYPROJECT PUBLIC ${IMGUI_DIR})
target_include_directories(MYPROJECT PUBLIC ${IMGUI_BACKENDS_DIR})
target_include_directories(MYPROJECT PUBLIC ${IMPLOT_DIR})
  
# Other Settings
 
set_property(TARGET MYPROJECT PROPERTY CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
 
# Other Paths    

set(SHADER_DIR "${CMAKE_SOURCE_DIR}/files/shaders/")     
target_compile_definitions(MYPROJECT PRIVATE SHADER_PATH="${SHADER_DIR}")

set(ASSET_DIR "${CMAKE_SOURCE_DIR}/files/assets/")
target_compile_definitions(MYPROJECT PRIVATE ASSET_PATH="${ASSET_DIR}")


# Include Libraries
 
 
include(FetchContent) 

# GLFW - Windows creation
FetchContent_Declare(  
	glfw   
	GIT_REPOSITORY https://github.com/glfw/glfw.git 
	GIT_TAG 	7b6aead9fb88b3623e3b3725ebb42670cbe4c579 #refs/tags/3.4
)
 
# Disable Wayland and force X11 on Linux 
if(UNIX AND NOT APPLE OR NOT WIN32 OR NOT MINGW OR NOT MSYS OR NOT CYGWIN)
	set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
	set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
endif()


FetchContent_MakeAvailable(glfw)   

target_link_libraries(MYPROJECT PRIVATE glfw)


# MPML - Math Library
FetchContent_Declare(
	MPML
	GIT_REPOSITORY https://github.com/Allosker/MPML.git 
)
 
FetchContent_MakeAvailable(MPML) 

target_link_libraries(MYPROJECT PRIVATE MPML::MPML)


