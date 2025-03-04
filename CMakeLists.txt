# CMakeList.txt : CMake project for AxC, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project ("AxC")
find_package(LLVM REQUIRED CONFIG)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libnames(LLVM_LIBS core native target targetparser support irreader bitreader)

# Add source to this project's executable.
add_executable (AxC "./src/AxC.cpp" "./src/AxC.h" "./src/AST.h" "./src/AST.cpp" "./src/Lexer.cpp" "./src/Lexer.h" "./src/Parser.h" "./src/Parser.cpp" "./src/TreeWalker.h" "./src/TreeWalker.cpp")
target_link_libraries(AxC ${LLVM_LIBS})

set_property(TARGET AxC PROPERTY CXX_STANDARD 20)