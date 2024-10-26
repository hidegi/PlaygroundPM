if(NOT NO_SP_X11)
    set(SP_X11_INCLUDE_DIRS
            "/usr/X11R6/include"
            "/usr/include/X11"
        )
    set(SP_X11_LIB_DIRS
            "/usr/X11R6/lib"
        )
else()
    set(SP_X11_INCLUDE_DIRS "")
    set(SP_X11_LIB_DIRS "")
endif()

find_path(SP_INCLUDE_DIR 
    NAMES
        SP/config.h
    HINTS
        "${SP_LOCATION}/include"
        "$ENV{SP_LOCATION}/include"
    PATHS
        "$ENV{PROGRAMFILES}/Playground/include"
        "${OPENGL_INCLUDE_DIR}"
        /usr/openwin/share/include
        /usr/openwin/include
        "${SP_X11_INCLUDE_DIRS}"
        /opt/graphics/OpenGL/include
        /opt/graphics/OpenGL/contrib/libplayground
        /usr/local/include
        /usr/include/GL
        /usr/include
    DOC 
        "The directory where SP/config.h resides"
)

if (WIN32)
    if(CYGWIN)
        find_library(SP_sp_LIBRARY 
				playground
            NAMES
                playground
				libplayground.a
				libplayground.dll
				libplayground.dll.a
            HINTS
				"${SP_LOCATION}/build/src"
				"${SP_LOCATION}/install/bin"
                "${SP_LOCATION}/lib"
                "${SP_LOCATION}/lib/x64"
                "$ENV{SP_LOCATION}/lib"
            PATHS
                "${OPENGL_LIBRARY_DIR}"
                /usr/lib
                /usr/lib/w32api
                /usr/local/lib
                "${SP_X11_LIB_DIRS}"
            DOC 
                "The Playground library"
        )
    else()
        find_library( SP_sp_LIBRARY
				playground
            NAMES 
				playground
				libplayground.a
				libplayground.dll
				libplayground.dll.a
            HINTS
				"${SP_LOCATION}/build/src"
				"${SP_LOCATION}/install/bin"
                "${SP_LOCATION}/lib"
                "${SP_LOCATION}/lib/x64"
                "${SP_LOCATION}/lib-msvc110"
                "${SP_LOCATION}/lib-vc2012"
                "$ENV{SP_LOCATION}/lib"
                "$ENV{SP_LOCATION}/lib/x64"
                "$ENV{SP_LOCATION}/lib-msvc110"
                "$ENV{SP_LOCATION}/lib-vc2012"
            PATHS
                "$ENV{PROGRAMFILES}/Playground/lib"
                "${OPENGL_LIBRARY_DIR}"
            DOC 
                "The Playground library"
        )
    endif()
else ()
    if (APPLE)
        find_library( SP_sp_LIBRARY playground
            NAMES 
                playground
            HINTS
                "${SP_LOCATION}/lib"
                "${SP_LOCATION}/lib/cocoa"
                "$ENV{SP_LOCATION}/lib"
                "$ENV{SP_LOCATION}/lib/cocoa"
            PATHS
                /usr/local/lib
        )
		
    else ()
        find_library( SP_sp_LIBRARY
            NAMES 
                playground
            HINTS
                "${SP_LOCATION}/lib"
                "$ENV{SP_LOCATION}/lib"
                "${SP_LOCATION}/lib/x11"
                "$ENV{SP_LOCATION}/lib/x11"
            PATHS
                /usr/lib64
                /usr/lib
                /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/local/lib64
                /usr/local/lib
                /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                /usr/openwin/lib
                "${SP_X11_LIB_DIRS}"
            DOC 
                "The Playground library"
        )
    endif (APPLE)
endif (WIN32)

set( SP_FOUND "NO" )

if(SP_INCLUDE_DIR)

    if(SP_sp_LIBRARY)
		set( SP_LIBRARIES "${SP_sp_LIBRARY}")
        set( SP_FOUND "YES" )
        set (SP_LIBRARY "${SP_LIBRARIES}")
        set (SP_INCLUDE_PATH "${SP_INCLUDE_DIR}")	
	else()
		message(FATAL_ERROR "To build the application, please specify SP_LOCATION!!")
    endif(SP_sp_LIBRARY)

    # Tease the SP_VERSION numbers from the lib headers
    function(parseVersion FILENAME VARNAME)
            
        set(PATTERN "^#define ${VARNAME}.*$")
        
        file(STRINGS "${SP_INCLUDE_DIR}/${FILENAME}" TMP REGEX ${PATTERN})
        
        string(REGEX MATCHALL "[0-9]+" TMP ${TMP})
        
        set(${VARNAME} ${TMP} PARENT_SCOPE)
        
    endfunction()


    if(EXISTS "${SP_INCLUDE_DIR}/SP/config.h")
        parseVersion(SP/config.h SP_VERSION_MAJOR)
        parseVersion(SP/config.h SP_VERSION_MINOR)
        parseVersion(SP/config.h SP_VERSION_PATCH)
    endif()

    if(${SP_VERSION_MAJOR} OR ${SP_VERSION_MINOR} OR ${SP_VERSION_PATCH})
        set(SP_VERSION "${SP_VERSION_MAJOR}.${SP_VERSION_MINOR}.${SP_VERSION_PATCH}")
        set(SP_VERSION_STRING "${SP_VERSION}")
        mark_as_advanced(SP_VERSION)
    endif()
    
endif(SP_INCLUDE_DIR)


if("${SP_INCLUDE_DIR}" STREQUAL "" OR "${SP_INCLUDE_DIR}" STREQUAL "SP_INCLUDE_DIR-NOTFOUND")
	message(FATAL_ERROR "To build the application, please specify SP_LOCATION!!")
endif()

if("${SP_LIBRARY}" STREQUAL "" OR "${SP_LIBRARY}" STREQUAL "SP_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "To build the application, please specify SP_LOCATION!!")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(playground
    REQUIRED_VARS
        SP_INCLUDE_DIR
        SP_LIBRARIES
    VERSION_VAR
        SP_VERSION
)

mark_as_advanced(
  SP_INCLUDE_DIR
  SP_LIBRARIES
  SP_sp_LIBRARY
)