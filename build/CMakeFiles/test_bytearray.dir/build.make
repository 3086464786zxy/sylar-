# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/liner/sylar

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/liner/sylar/build

# Include any dependencies generated for this target.
include CMakeFiles/test_bytearray.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/test_bytearray.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/test_bytearray.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_bytearray.dir/flags.make

CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o: CMakeFiles/test_bytearray.dir/flags.make
CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o: /home/liner/sylar/source/test_bytearray.cpp
CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o: CMakeFiles/test_bytearray.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/liner/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"source/test_bytearray.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o -MF CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o.d -o CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o -c /home/liner/sylar/source/test_bytearray.cpp

CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"source/test_bytearray.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/liner/sylar/source/test_bytearray.cpp > CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.i

CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) -D__FILE__=\"source/test_bytearray.cpp\" $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/liner/sylar/source/test_bytearray.cpp -o CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.s

# Object files for target test_bytearray
test_bytearray_OBJECTS = \
"CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o"

# External object files for target test_bytearray
test_bytearray_EXTERNAL_OBJECTS =

/home/liner/sylar/bin/test_bytearray: CMakeFiles/test_bytearray.dir/source/test_bytearray.cpp.o
/home/liner/sylar/bin/test_bytearray: CMakeFiles/test_bytearray.dir/build.make
/home/liner/sylar/bin/test_bytearray: /home/liner/sylar/lib/libsylar.so
/home/liner/sylar/bin/test_bytearray: /usr/local/lib/libyaml-cpp.so
/home/liner/sylar/bin/test_bytearray: CMakeFiles/test_bytearray.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/liner/sylar/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/liner/sylar/bin/test_bytearray"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_bytearray.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_bytearray.dir/build: /home/liner/sylar/bin/test_bytearray
.PHONY : CMakeFiles/test_bytearray.dir/build

CMakeFiles/test_bytearray.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_bytearray.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_bytearray.dir/clean

CMakeFiles/test_bytearray.dir/depend:
	cd /home/liner/sylar/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/liner/sylar /home/liner/sylar /home/liner/sylar/build /home/liner/sylar/build /home/liner/sylar/build/CMakeFiles/test_bytearray.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/test_bytearray.dir/depend

