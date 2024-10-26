link_libraries(${SP_LIBRARY})
include_directories(${CMAKE_SOURCE_DIR}/include ${SP_INCLUDE_DIR})

if(WIN32)
	set(ICON icon.rc)
elseif(APPLE)

endif(WIN32)

LIST(APPEND SOURCE_FILES 
	main.cpp
	PM/pacman.cpp
	PM/board.cpp
	)

add_executable(pacman ${SOURCE_FILES} ${ICON})

if (MSVC)
    set_target_properties(pacman PROPERTIES
                          LINK_FLAGS "/ENTRY:mainCRTStartup")
elseif (CMAKE_C_SIMULATE_ID STREQUAL "MSVC")
    set_target_properties(pacman PROPERTIES
                          LINK_FLAGS "-Wl,/entry:mainCRTStartup")
endif()