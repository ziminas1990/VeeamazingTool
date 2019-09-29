cmake_minimum_required(VERSION 3.0)
project(VeeamazingTool)

set(CMAKE_BUILD_TYPE Debug)

find_package(Boost 1.65 REQUIRED COMPONENTS fiber)

add_executable(VeeamazingTool
  main.cpp FileBlocksStream.cpp FileDevice.cpp Worker.cpp SignatureFile.cpp
  Configuration.cpp)
set_target_properties(VeeamazingTool PROPERTIES
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  COMPILE_OPTIONS "-Wall;-Wextra;-Werror=return-type"
)

target_link_libraries(VeeamazingTool ${Boost_LIBRARIES})
