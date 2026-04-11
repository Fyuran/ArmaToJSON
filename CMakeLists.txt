cmake_minimum_required(VERSION 4.2)
project (ArmaToJSON
        LANGUAGES CXX
)

include(CMakePrintHelpers)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("test")

add_library (ArmaToJSON SHARED)
target_sources (ArmaToJSON PRIVATE 
	"include/dllmain.hpp"
	"src/dllmain.cpp"
	"include/manager.hpp"
	"src/manager.cpp"
	"include/functions.hpp"
	"src/functions.cpp"
	"include/logger.hpp"
	"src/logger.cpp"
)

set_target_properties(ArmaToJSON PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(BUILD_STATIC)
	if(MINGW)
		target_link_options(ArmaToJSON PUBLIC -static -static-libgcc -static-libstdc++)
	else()
		target_link_options(ArmaToJSON PUBLIC -static-libgcc -static-libstdc++)
	endif()
endif()

if(CMAKE_CROSSCOMPILING)
	# where is the target environment
	list (APPEND CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)

	set(CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES usr/x86_64-w64-mingw32/include)
	set(CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES usr/x86_64-w64-mingw32/include)

	# search for programs in the build host directories
	set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	# for libraries and headers in the target directories
	set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

	# Make sure Qt can be detected by CMake
	set (QT_BINARY_DIR /usr/x86_64-w64-mingw32/bin /usr/bin)
	set (QT_INCLUDE_DIRS_NO_SYSTEM ON)
	set (QT_HOST_PATH "/usr" CACHE PATH "host path for Qt")

	# set the resource compiler (RHBZ #652435)
	set (CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)
	set (CMAKE_MC_COMPILER x86_64-w64-mingw32-windmc)

	# override boost thread component suffix as mingw-w64-boost is compiled with threadapi=win32
	set (Boost_THREADAPI win32)

	# These are needed for compiling lapack (RHBZ #753906)
	set (CMAKE_Fortran_COMPILER x86_64-w64-mingw32-gfortran)
	set (CMAKE_AR:FILEPATH x86_64-w64-mingw32-ar)
	set (CMAKE_RANLIB:FILEPATH x86_64-w64-mingw32-ranlib)
endif()

target_include_directories(ArmaToJSON PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include/>)

target_compile_features(ArmaToJSON PUBLIC cxx_std_20)

find_package(Boost REQUIRED COMPONENTS date_time)
target_link_libraries(ArmaToJSON PUBLIC Boost::date_time)

find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(ArmaToJSON PUBLIC nlohmann_json::nlohmann_json)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(ArmaToJSON PUBLIC fmt::fmt)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(ArmaToJSON PUBLIC spdlog::spdlog)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,_x64,>" PREFIX "btc_")

cmake_print_properties(
  TARGETS ArmaToJSON
  PROPERTIES INCLUDE_DIRECTORIES OUTPUT_NAME
)

cmake_print_variables(
	CMAKE_CROSSCOMPILING 
	CMAKE_HOST_SYSTEM_NAME 
	CMAKE_SYSTEM_NAME
	BUILD_STATIC
)

add_custom_command(
    TARGET ArmaToJSON
    POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArmaToJSON> "${PROJECT_SOURCE_DIR}/build/${PRESET_NAME}/test/$<CONFIG>/"
    COMMENT "Moving lib to Test Folder"
)

if(DEFINED ENV{ARMA3} AND NOT "$ENV{ARMA3}" STREQUAL "")
add_custom_command(
	TARGET ArmaToJSON
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArmaToJSON> $ENV{ARMA3}
    COMMENT "Moving lib to Arma 3 folder")
endif()

if(DEFINED ENV{btc_toolchain} AND NOT "$ENV{btc_toolchain}" STREQUAL "" AND $<$<CONFIG:Release>:TRUE,FALSE>)
add_custom_command(
	TARGET ArmaToJSON
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:ArmaToJSON> $ENV{btc_toolchain}/json/
    COMMENT "Moving lib to btc_toolchain folder")
endif()