cmake_minimum_required(VERSION 3.27.7)

project(sylar)

include (cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -fPIC -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined -Wno-deprecated-declarations")

set(LIB_SRC
		sylar/address.cpp
        sylar/config.cpp
		sylar/fiber.cpp
		sylar/fd_manager.cpp
		sylar/hook.cpp
        sylar/log.cpp
		sylar/thread.cpp
		sylar/timer.cpp
		sylar/scheduler.cpp
		sylar/iomanager.cpp
        sylar/util.cpp
)

include_directories(./sylar)
include_directories(/usr/local/include/yaml-cpp /usr/local/include)

link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)
message("...", ${YAMLCPP})

add_library(sylar SHARED ${LIB_SRC})
force_redefine_file_macro_for_sources(sylar)	# __FILE__ 	 

set(LIB_LIB 
		sylar 
		dl
		pthread
		${YAMLCPP}
	) 

add_executable(test source/test.cpp )
add_dependencies(test sylar)
force_redefine_file_macro_for_sources(test)		# __FILE__ 
target_link_libraries(test sylar ${LIB_LIB})

add_executable(test_config source/test_config.cpp)
add_dependencies(test_config sylar) 
force_redefine_file_macro_for_sources(test_config) 	# __FILE__ 
#target_link_libraries(test_config sylar -L/usr/local/lib -lyaml-cpp) 
target_link_libraries(test_config sylar ${LIB_LIB})

add_executable(test_thread source/test_thread.cpp )
add_dependencies(test_thread sylar)
force_redefine_file_macro_for_sources(test_thread)		# __FILE__ 
target_link_libraries(test_thread sylar ${LIB_LIB})

add_executable(test_pipe source/test_pipe.cpp )
add_dependencies(test_pipe sylar)
force_redefine_file_macro_for_sources(test_pipe)		# __FILE__ 
target_link_libraries(test_pipe sylar ${LIB_LIB})

add_executable(test_util source/test_util.cpp )
add_dependencies(test_util sylar)
force_redefine_file_macro_for_sources(test_util)		# __FILE__ 
target_link_libraries(test_util sylar ${LIB_LIB})

add_executable(test_fiber source/test_fiber.cpp )
add_dependencies(test_fiber sylar)
force_redefine_file_macro_for_sources(test_fiber)		# __FILE__ 
target_link_libraries(test_fiber sylar ${LIB_LIB})

add_executable(test_scheduler source/test_scheduler.cpp )
add_dependencies(test_scheduler sylar)
force_redefine_file_macro_for_sources(test_scheduler)		# __FILE__ 
target_link_libraries(test_scheduler sylar ${LIB_LIB})

add_executable(test_iomanager source/test_iomanager.cpp )
add_dependencies(test_iomanager sylar)
force_redefine_file_macro_for_sources(test_iomanager)		# __FILE__ 
target_link_libraries(test_iomanager sylar ${LIB_LIB})

add_executable(test_hook source/test_hook.cpp )
add_dependencies(test_hook sylar)
force_redefine_file_macro_for_sources(test_hook)		# __FILE__ 
target_link_libraries(test_hook sylar ${LIB_LIB})

add_executable(test_address source/test_address.cpp )
add_dependencies(test_address sylar)
force_redefine_file_macro_for_sources(test_address)		# __FILE__ 
target_link_libraries(test_address sylar ${LIB_LIB})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
