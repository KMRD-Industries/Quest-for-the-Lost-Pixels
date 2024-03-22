# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_SOURCE_DIR = /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1

# Include any dependencies generated for this target.
include CMakeFiles/zlibstatic.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/zlibstatic.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/zlibstatic.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/zlibstatic.dir/flags.make

CMakeFiles/zlibstatic.dir/adler32.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/adler32.c.o: adler32.c
CMakeFiles/zlibstatic.dir/adler32.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/zlibstatic.dir/adler32.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/adler32.c.o -MF CMakeFiles/zlibstatic.dir/adler32.c.o.d -o CMakeFiles/zlibstatic.dir/adler32.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/adler32.c

CMakeFiles/zlibstatic.dir/adler32.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/adler32.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/adler32.c > CMakeFiles/zlibstatic.dir/adler32.c.i

CMakeFiles/zlibstatic.dir/adler32.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/adler32.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/adler32.c -o CMakeFiles/zlibstatic.dir/adler32.c.s

CMakeFiles/zlibstatic.dir/compress.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/compress.c.o: compress.c
CMakeFiles/zlibstatic.dir/compress.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/zlibstatic.dir/compress.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/compress.c.o -MF CMakeFiles/zlibstatic.dir/compress.c.o.d -o CMakeFiles/zlibstatic.dir/compress.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/compress.c

CMakeFiles/zlibstatic.dir/compress.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/compress.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/compress.c > CMakeFiles/zlibstatic.dir/compress.c.i

CMakeFiles/zlibstatic.dir/compress.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/compress.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/compress.c -o CMakeFiles/zlibstatic.dir/compress.c.s

CMakeFiles/zlibstatic.dir/crc32.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/crc32.c.o: crc32.c
CMakeFiles/zlibstatic.dir/crc32.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/zlibstatic.dir/crc32.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/crc32.c.o -MF CMakeFiles/zlibstatic.dir/crc32.c.o.d -o CMakeFiles/zlibstatic.dir/crc32.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/crc32.c

CMakeFiles/zlibstatic.dir/crc32.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/crc32.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/crc32.c > CMakeFiles/zlibstatic.dir/crc32.c.i

CMakeFiles/zlibstatic.dir/crc32.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/crc32.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/crc32.c -o CMakeFiles/zlibstatic.dir/crc32.c.s

CMakeFiles/zlibstatic.dir/deflate.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/deflate.c.o: deflate.c
CMakeFiles/zlibstatic.dir/deflate.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/zlibstatic.dir/deflate.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/deflate.c.o -MF CMakeFiles/zlibstatic.dir/deflate.c.o.d -o CMakeFiles/zlibstatic.dir/deflate.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/deflate.c

CMakeFiles/zlibstatic.dir/deflate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/deflate.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/deflate.c > CMakeFiles/zlibstatic.dir/deflate.c.i

CMakeFiles/zlibstatic.dir/deflate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/deflate.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/deflate.c -o CMakeFiles/zlibstatic.dir/deflate.c.s

CMakeFiles/zlibstatic.dir/gzclose.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/gzclose.c.o: gzclose.c
CMakeFiles/zlibstatic.dir/gzclose.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/zlibstatic.dir/gzclose.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/gzclose.c.o -MF CMakeFiles/zlibstatic.dir/gzclose.c.o.d -o CMakeFiles/zlibstatic.dir/gzclose.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzclose.c

CMakeFiles/zlibstatic.dir/gzclose.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/gzclose.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzclose.c > CMakeFiles/zlibstatic.dir/gzclose.c.i

CMakeFiles/zlibstatic.dir/gzclose.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/gzclose.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzclose.c -o CMakeFiles/zlibstatic.dir/gzclose.c.s

CMakeFiles/zlibstatic.dir/gzlib.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/gzlib.c.o: gzlib.c
CMakeFiles/zlibstatic.dir/gzlib.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/zlibstatic.dir/gzlib.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/gzlib.c.o -MF CMakeFiles/zlibstatic.dir/gzlib.c.o.d -o CMakeFiles/zlibstatic.dir/gzlib.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzlib.c

CMakeFiles/zlibstatic.dir/gzlib.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/gzlib.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzlib.c > CMakeFiles/zlibstatic.dir/gzlib.c.i

CMakeFiles/zlibstatic.dir/gzlib.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/gzlib.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzlib.c -o CMakeFiles/zlibstatic.dir/gzlib.c.s

CMakeFiles/zlibstatic.dir/gzread.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/gzread.c.o: gzread.c
CMakeFiles/zlibstatic.dir/gzread.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/zlibstatic.dir/gzread.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/gzread.c.o -MF CMakeFiles/zlibstatic.dir/gzread.c.o.d -o CMakeFiles/zlibstatic.dir/gzread.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzread.c

CMakeFiles/zlibstatic.dir/gzread.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/gzread.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzread.c > CMakeFiles/zlibstatic.dir/gzread.c.i

CMakeFiles/zlibstatic.dir/gzread.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/gzread.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzread.c -o CMakeFiles/zlibstatic.dir/gzread.c.s

CMakeFiles/zlibstatic.dir/gzwrite.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/gzwrite.c.o: gzwrite.c
CMakeFiles/zlibstatic.dir/gzwrite.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/zlibstatic.dir/gzwrite.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/gzwrite.c.o -MF CMakeFiles/zlibstatic.dir/gzwrite.c.o.d -o CMakeFiles/zlibstatic.dir/gzwrite.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzwrite.c

CMakeFiles/zlibstatic.dir/gzwrite.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/gzwrite.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzwrite.c > CMakeFiles/zlibstatic.dir/gzwrite.c.i

CMakeFiles/zlibstatic.dir/gzwrite.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/gzwrite.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/gzwrite.c -o CMakeFiles/zlibstatic.dir/gzwrite.c.s

CMakeFiles/zlibstatic.dir/inflate.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/inflate.c.o: inflate.c
CMakeFiles/zlibstatic.dir/inflate.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/zlibstatic.dir/inflate.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/inflate.c.o -MF CMakeFiles/zlibstatic.dir/inflate.c.o.d -o CMakeFiles/zlibstatic.dir/inflate.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inflate.c

CMakeFiles/zlibstatic.dir/inflate.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/inflate.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inflate.c > CMakeFiles/zlibstatic.dir/inflate.c.i

CMakeFiles/zlibstatic.dir/inflate.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/inflate.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inflate.c -o CMakeFiles/zlibstatic.dir/inflate.c.s

CMakeFiles/zlibstatic.dir/infback.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/infback.c.o: infback.c
CMakeFiles/zlibstatic.dir/infback.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Building C object CMakeFiles/zlibstatic.dir/infback.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/infback.c.o -MF CMakeFiles/zlibstatic.dir/infback.c.o.d -o CMakeFiles/zlibstatic.dir/infback.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/infback.c

CMakeFiles/zlibstatic.dir/infback.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/infback.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/infback.c > CMakeFiles/zlibstatic.dir/infback.c.i

CMakeFiles/zlibstatic.dir/infback.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/infback.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/infback.c -o CMakeFiles/zlibstatic.dir/infback.c.s

CMakeFiles/zlibstatic.dir/inftrees.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/inftrees.c.o: inftrees.c
CMakeFiles/zlibstatic.dir/inftrees.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_11) "Building C object CMakeFiles/zlibstatic.dir/inftrees.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/inftrees.c.o -MF CMakeFiles/zlibstatic.dir/inftrees.c.o.d -o CMakeFiles/zlibstatic.dir/inftrees.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inftrees.c

CMakeFiles/zlibstatic.dir/inftrees.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/inftrees.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inftrees.c > CMakeFiles/zlibstatic.dir/inftrees.c.i

CMakeFiles/zlibstatic.dir/inftrees.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/inftrees.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inftrees.c -o CMakeFiles/zlibstatic.dir/inftrees.c.s

CMakeFiles/zlibstatic.dir/inffast.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/inffast.c.o: inffast.c
CMakeFiles/zlibstatic.dir/inffast.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_12) "Building C object CMakeFiles/zlibstatic.dir/inffast.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/inffast.c.o -MF CMakeFiles/zlibstatic.dir/inffast.c.o.d -o CMakeFiles/zlibstatic.dir/inffast.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inffast.c

CMakeFiles/zlibstatic.dir/inffast.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/inffast.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inffast.c > CMakeFiles/zlibstatic.dir/inffast.c.i

CMakeFiles/zlibstatic.dir/inffast.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/inffast.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/inffast.c -o CMakeFiles/zlibstatic.dir/inffast.c.s

CMakeFiles/zlibstatic.dir/trees.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/trees.c.o: trees.c
CMakeFiles/zlibstatic.dir/trees.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_13) "Building C object CMakeFiles/zlibstatic.dir/trees.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/trees.c.o -MF CMakeFiles/zlibstatic.dir/trees.c.o.d -o CMakeFiles/zlibstatic.dir/trees.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/trees.c

CMakeFiles/zlibstatic.dir/trees.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/trees.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/trees.c > CMakeFiles/zlibstatic.dir/trees.c.i

CMakeFiles/zlibstatic.dir/trees.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/trees.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/trees.c -o CMakeFiles/zlibstatic.dir/trees.c.s

CMakeFiles/zlibstatic.dir/uncompr.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/uncompr.c.o: uncompr.c
CMakeFiles/zlibstatic.dir/uncompr.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_14) "Building C object CMakeFiles/zlibstatic.dir/uncompr.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/uncompr.c.o -MF CMakeFiles/zlibstatic.dir/uncompr.c.o.d -o CMakeFiles/zlibstatic.dir/uncompr.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/uncompr.c

CMakeFiles/zlibstatic.dir/uncompr.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/uncompr.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/uncompr.c > CMakeFiles/zlibstatic.dir/uncompr.c.i

CMakeFiles/zlibstatic.dir/uncompr.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/uncompr.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/uncompr.c -o CMakeFiles/zlibstatic.dir/uncompr.c.s

CMakeFiles/zlibstatic.dir/zutil.c.o: CMakeFiles/zlibstatic.dir/flags.make
CMakeFiles/zlibstatic.dir/zutil.c.o: zutil.c
CMakeFiles/zlibstatic.dir/zutil.c.o: CMakeFiles/zlibstatic.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_15) "Building C object CMakeFiles/zlibstatic.dir/zutil.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/zlibstatic.dir/zutil.c.o -MF CMakeFiles/zlibstatic.dir/zutil.c.o.d -o CMakeFiles/zlibstatic.dir/zutil.c.o -c /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/zutil.c

CMakeFiles/zlibstatic.dir/zutil.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing C source to CMakeFiles/zlibstatic.dir/zutil.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/zutil.c > CMakeFiles/zlibstatic.dir/zutil.c.i

CMakeFiles/zlibstatic.dir/zutil.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling C source to assembly CMakeFiles/zlibstatic.dir/zutil.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/zutil.c -o CMakeFiles/zlibstatic.dir/zutil.c.s

# Object files for target zlibstatic
zlibstatic_OBJECTS = \
"CMakeFiles/zlibstatic.dir/adler32.c.o" \
"CMakeFiles/zlibstatic.dir/compress.c.o" \
"CMakeFiles/zlibstatic.dir/crc32.c.o" \
"CMakeFiles/zlibstatic.dir/deflate.c.o" \
"CMakeFiles/zlibstatic.dir/gzclose.c.o" \
"CMakeFiles/zlibstatic.dir/gzlib.c.o" \
"CMakeFiles/zlibstatic.dir/gzread.c.o" \
"CMakeFiles/zlibstatic.dir/gzwrite.c.o" \
"CMakeFiles/zlibstatic.dir/inflate.c.o" \
"CMakeFiles/zlibstatic.dir/infback.c.o" \
"CMakeFiles/zlibstatic.dir/inftrees.c.o" \
"CMakeFiles/zlibstatic.dir/inffast.c.o" \
"CMakeFiles/zlibstatic.dir/trees.c.o" \
"CMakeFiles/zlibstatic.dir/uncompr.c.o" \
"CMakeFiles/zlibstatic.dir/zutil.c.o"

# External object files for target zlibstatic
zlibstatic_EXTERNAL_OBJECTS =

libz.a: CMakeFiles/zlibstatic.dir/adler32.c.o
libz.a: CMakeFiles/zlibstatic.dir/compress.c.o
libz.a: CMakeFiles/zlibstatic.dir/crc32.c.o
libz.a: CMakeFiles/zlibstatic.dir/deflate.c.o
libz.a: CMakeFiles/zlibstatic.dir/gzclose.c.o
libz.a: CMakeFiles/zlibstatic.dir/gzlib.c.o
libz.a: CMakeFiles/zlibstatic.dir/gzread.c.o
libz.a: CMakeFiles/zlibstatic.dir/gzwrite.c.o
libz.a: CMakeFiles/zlibstatic.dir/inflate.c.o
libz.a: CMakeFiles/zlibstatic.dir/infback.c.o
libz.a: CMakeFiles/zlibstatic.dir/inftrees.c.o
libz.a: CMakeFiles/zlibstatic.dir/inffast.c.o
libz.a: CMakeFiles/zlibstatic.dir/trees.c.o
libz.a: CMakeFiles/zlibstatic.dir/uncompr.c.o
libz.a: CMakeFiles/zlibstatic.dir/zutil.c.o
libz.a: CMakeFiles/zlibstatic.dir/build.make
libz.a: CMakeFiles/zlibstatic.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_16) "Linking C static library libz.a"
	$(CMAKE_COMMAND) -P CMakeFiles/zlibstatic.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zlibstatic.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/zlibstatic.dir/build: libz.a
.PHONY : CMakeFiles/zlibstatic.dir/build

CMakeFiles/zlibstatic.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/zlibstatic.dir/cmake_clean.cmake
.PHONY : CMakeFiles/zlibstatic.dir/clean

CMakeFiles/zlibstatic.dir/depend:
	cd /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1 /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1 /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1 /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1 /home/dominiq/Desktop/KMDR/atlas_creator/Moony-TexturePacker/source/zlib-1.3.1/CMakeFiles/zlibstatic.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/zlibstatic.dir/depend

