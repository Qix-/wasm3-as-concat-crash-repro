project (wasm3-as-concat-crash-repro)
cmake_minimum_required (VERSION 3.2)

add_subdirectory (wasm3)

add_executable (repro main.cc)
target_link_libraries (repro PRIVATE m3)
