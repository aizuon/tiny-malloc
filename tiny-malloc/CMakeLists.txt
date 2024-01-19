set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS "${BASE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS "${BASE_DIR}/*.hpp")
list(FILTER HEADER_FILES EXCLUDE REGEX "${BASE_DIR}/pch.hpp$")

add_executable(tiny-malloc ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(tiny-malloc PUBLIC "${BASE_DIR}")

target_precompile_headers(tiny-malloc PRIVATE "${BASE_DIR}/pch.hpp")
