cmake_minimum_required (VERSION 3.10)
project ("01-quickstart")

find_package(OpenMAMA REQUIRED PATHS ${CMAKE_SOURCE_DIR})

add_executable(quickstartc quickstart.c)
target_link_libraries(quickstartc PRIVATE OpenMAMA::mama)
