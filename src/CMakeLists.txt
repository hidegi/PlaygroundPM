link_libraries(${SP_LIBRARY})
include_directories(${CMAKE_SOURCE_DIR}/include ${SP_INCLUDE_DIR})

add_executable(pacman main.cpp)

if (MSVC)
    set_target_properties(pacman PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    set_target_properties(pacman PROPERTIES
                          LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()