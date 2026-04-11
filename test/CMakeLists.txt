cmake_minimum_required(VERSION 4.2)
project (ArmaToJSON_test
        LANGUAGES CXX
)

include(CMakePrintHelpers)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(ArmaToJSON_test)
target_sources (ArmaToJSON_test PRIVATE 
	"include/main.hpp"
	"src/main.cpp"
)
target_include_directories(ArmaToJSON_test PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/> $<INSTALL_INTERFACE:include/>)
set_target_properties(ArmaToJSON_test PROPERTIES
    EXCLUDE_FROM_ALL $<$<CONFIG:Release>:1>
)
target_compile_features(ArmaToJSON_test PRIVATE cxx_std_20)
target_link_libraries(ArmaToJSON_test PRIVATE ArmaToJSON)
