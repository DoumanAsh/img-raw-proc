file(GLOB_RECURSE SRC "*.cpp")
add_executable(${CMAKE_PROJECT_NAME} ${SRC})

find_package(libpng REQUIRED)
find_package(libraw CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
target_link_libraries(
    ${CMAKE_PROJECT_NAME}
    PRIVATE
    png
    libraw::raw_r
    fmt::fmt-header-only
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_status OUTPUT output)
if (ipo_status)
    set_property(TARGET ${CMAKE_PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
