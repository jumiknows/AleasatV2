# Metadata
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER_ID GNU)

# Tools
set(tools $ENV{TI_CGT_ARM_DIR})
set(CMAKE_C_COMPILER ${tools}/bin/armcl)
set(CMAKE_ASM_COMPILER ${CMAKE_C_COMPILER})

set(CMAKE_OBJCOPY ${tools}/bin/armobjcopy CACHE INTERNAL "objcopy tool")
set(CMAKE_SIZE_UTIL ${tools}/bin/armsize CACHE INTERNAL "size tool")
set(CMAKE_HEX ${tools}/bin/armhex CACHE INTERNAL "hex tool")

# Filename Suffixes
set(CMAKE_EXECUTABLE_SUFFIX_C   .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

# Compiler Flags
set(CMAKE_C_FLAGS_INIT "-mv7R4 --code_state=32 --float_support=VFPv3D16 --c99 --abi=eabi --enum_type=packed --gen_func_subsections=on --gen_data_subsections=on -g --emit_warnings_as_errors --issue_remarks --verbose_diagnostics --display_error_number")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS_INIT}" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS_INIT ${CMAKE_C_FLAGS_INIT})
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS_INIT}" CACHE STRING "" FORCE)

# Linker Flags
set(CMAKE_EXE_LINKER_FLAGS_INIT "--priority --rom_model --be32 --warn_sections") # --priority is important here to allow object files / earlier libraries to override symbols from later libraries
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS_INIT}" CACHE STRING "" FORCE)

# Standard Libraries
set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES ${tools}/include)
set(CMAKE_C_STANDARD_LIBRARIES ${tools}/lib/rtsv7R4_T_be_v3D16_eabi.lib)

# Root Path
set(CMAKE_FIND_ROOT_PATH ${tools}/include
                         ${tools}/lib)

# adjust the default behavior of the FIND_XXX() commands:
# search for headers and libraries in the target environment,
# search for programs in the host environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# Tell CMake to build a static library test program instead of an executable which obviously can't run
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
