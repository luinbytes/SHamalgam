cmake_minimum_required(VERSION 3.25)
project(SHamalgam LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE SHAMALGAM_CPP CONFIGURE_DEPENDS
	Amalgam/src/*.cpp
	Amalgam/include/ImGui/*.cpp
)

list(FILTER SHAMALGAM_CPP EXCLUDE REGEX ".*/src/DllMain\\.cpp$")
list(FILTER SHAMALGAM_CPP EXCLUDE REGEX ".*/src/Hooks/Direct3DDevice9\\.cpp$")
list(FILTER SHAMALGAM_CPP EXCLUDE REGEX ".*/include/ImGui/imgui_impl_dx9\\.cpp$")
list(FILTER SHAMALGAM_CPP EXCLUDE REGEX ".*/include/ImGui/imgui_impl_win32\\.cpp$")

add_library(SHamalgam SHARED ${SHAMALGAM_CPP})

target_include_directories(SHamalgam PRIVATE
	Amalgam/include/platform/linux
	Amalgam/include
	Amalgam/src
)

target_compile_definitions(SHamalgam PRIVATE
	SHAMALGAM_LINUX=1
	AMALGAM_CUSTOM_FONTS=1
	__CONFIGURATION__="Linux"
)

target_compile_options(SHamalgam PRIVATE
	-include platform/linux/Platform.h
	-fms-extensions
	-fdeclspec
	-Wno-microsoft-cast
	-Wno-invalid-token-paste
	-Wno-c++11-narrowing
	-Wno-ignored-attributes
	-Wno-deprecated-declarations
	-Wno-unused-command-line-argument
)

target_link_libraries(SHamalgam PRIVATE dl pthread)
