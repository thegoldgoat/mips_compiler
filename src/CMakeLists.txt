cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_executable(mips_compiler main.cpp)

# add_library(Registers STATIC Registers/Registers.cpp)

# internal libraries
#target_link_libraries(mips_emulator
#  Decoder
#)

target_compile_features(mips_compiler PUBLIC cxx_std_17)
