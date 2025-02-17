# CMakeList.txt : CMake project for AxC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("AxC")

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS core native target targetparser support irreader bitreader)

# Add source to this project's executable.
add_executable (AxC "AxC.cpp" "AxC.h")
target_link_libraries(AxC ${LLVM_LIBS})

set_property(TARGET AxC PROPERTY CXX_STANDARD 20)


# TODO: Add tests and install targets if needed.
