# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rcidadef/repos/Waves/Waves/indexer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rcidadef/repos/Waves/Waves/indexer/build

# Include any dependencies generated for this target.
include src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/flags.make

src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o: src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/flags.make
src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o: ../src/run/geraSkiplistBlockMaxFIX.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rcidadef/repos/Waves/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o"
	cd /home/rcidadef/repos/Waves/Waves/indexer/build/src && /usr/bin/g++-10  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o -c /home/rcidadef/repos/Waves/Waves/indexer/src/run/geraSkiplistBlockMaxFIX.cc

src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.i"
	cd /home/rcidadef/repos/Waves/Waves/indexer/build/src && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rcidadef/repos/Waves/Waves/indexer/src/run/geraSkiplistBlockMaxFIX.cc > CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.i

src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.s"
	cd /home/rcidadef/repos/Waves/Waves/indexer/build/src && /usr/bin/g++-10 $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rcidadef/repos/Waves/Waves/indexer/src/run/geraSkiplistBlockMaxFIX.cc -o CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.s

# Object files for target geraSkiplistBlockMaxFIX
geraSkiplistBlockMaxFIX_OBJECTS = \
"CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o"

# External object files for target geraSkiplistBlockMaxFIX
geraSkiplistBlockMaxFIX_EXTERNAL_OBJECTS =

src/geraSkiplistBlockMaxFIX: src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/run/geraSkiplistBlockMaxFIX.o
src/geraSkiplistBlockMaxFIX: src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/build.make
src/geraSkiplistBlockMaxFIX: src/libindexer.so
src/geraSkiplistBlockMaxFIX: src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rcidadef/repos/Waves/Waves/indexer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable geraSkiplistBlockMaxFIX"
	cd /home/rcidadef/repos/Waves/Waves/indexer/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/geraSkiplistBlockMaxFIX.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/build: src/geraSkiplistBlockMaxFIX

.PHONY : src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/build

src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/clean:
	cd /home/rcidadef/repos/Waves/Waves/indexer/build/src && $(CMAKE_COMMAND) -P CMakeFiles/geraSkiplistBlockMaxFIX.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/clean

src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/depend:
	cd /home/rcidadef/repos/Waves/Waves/indexer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rcidadef/repos/Waves/Waves/indexer /home/rcidadef/repos/Waves/Waves/indexer/src /home/rcidadef/repos/Waves/Waves/indexer/build /home/rcidadef/repos/Waves/Waves/indexer/build/src /home/rcidadef/repos/Waves/Waves/indexer/build/src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/geraSkiplistBlockMaxFIX.dir/depend

