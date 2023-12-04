file(GLOB_RECURSE SRC "*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC})

find_package(libpng REQUIRED)
find_package(libraw CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

if (UNIX)
    set(PNG_LIB "png")
else()
    set(PNG_LIB "png_static")
endif()

target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    ${PNG_LIB}
    libraw::raw_r
    fmt::fmt-header-only
)

add_dependencies(${CMAKE_PROJECT_NAME} librtprocess)
ExternalProject_Get_property(librtprocess SOURCE_DIR)
message("SOURCE_DIR librtprocess = ${SOURCE_DIR}")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${SOURCE_DIR}/src/include)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_status OUTPUT output)
if (ipo_status)
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
