
cmake_minimum_required (VERSION 3.10)
project ("small-state-machine")

# Generic well-known values most CMake users might use

set (BUILD_SHARED_LIBS
     ON
     CACHE BOOL "Build shared libraries")

# SSM project values

set (SSM_BUILD_TESTS
     OFF
     CACHE BOOL "Build tests (but doesn't run them) using the Boost.Test library")

set (SSM_BOOST_PATH
     ""
     CACHE PATH "Additional path to search for boost (will be appended to CMAKE_PREFIX_PATH)")

set (SSM_MAINLIB_NAME
     "ssm"
     CACHE STRING "The name of the main library")

set (SSM_TESTEXEC_NAME
     "ssm-unit-test"
     CACHE STRING "The name of the test executable")

set (SSM_BUILD_STATIC_AND_SHARED_LIBS
     OFF
     CACHE BOOL "Build static and shared libraries (this will ignore BUILD_SHARED_LIBS)")

# We need CPP 14 at the very least

set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

# A function to add a compilation flag if it doesn't exist

function (add_flag CMAKE_VAR NEW_FLAG)
	if (${${CMAKE_VAR}} MATCHES ${NEW_FLAG})
		return ()
	else ()
		set (${CMAKE_VAR} "${${CMAKE_VAR}} ${NEW_FLAG}" PARENT_SCOPE)
	endif ()
endfunction ()

# A function to edit compilation flags

function (add_or_replace_flag CMAKE_VAR OLD_FLAG_REGEX NEW_FLAG)
	if (${${CMAKE_VAR}} MATCHES ${OLD_FLAG_REGEX})
		string (REGEX REPLACE ${OLD_FLAG_REGEX} ${NEW_FLAG} ${CMAKE_VAR} ${${CMAKE_VAR}})
		set (${CMAKE_VAR} ${${CMAKE_VAR}} PARENT_SCOPE)
	else ()
		set (${CMAKE_VAR} "${${CMAKE_VAR}} ${NEW_FLAG}" PARENT_SCOPE)
	endif()
endfunction ()



#add_or_replace_flag (CMAKE_CXX_FLAGS "/W[0-3]" "/W4")
#add_flag (CMAKE_CXX_FLAGS "/toto")
#message ("here is CMAKE_CXX_FLAGS again but from main scope: ${CMAKE_CXX_FLAGS}")




# set (MAINLIB_STATIC_TARGET "ssm-static")
# set (MAINLIB_SHARED_TARGET "ssm-shared")
# set (SSM_MAINLIB_NAME "ssm")

set (MAINLIB_SRC_FILES
     "src/progid.cpp"
     "src/state.cpp"
     "src/state_machine.cpp")

set (MAINLIB_INCLUDE_DIRS "include")

# Handle static/shared build combinations.

set (BUILD_STATIC OFF)
set (BUILD_SHARED OFF)

if (SSM_BUILD_STATIC_AND_SHARED_LIBS)
	
	# We ignore BUILD_SHARED_LIBS and build everything

	set (BUILD_STATIC ON)
	set (BUILD_SHARED ON)

else ()

	if (BUILD_SHARED_LIBS)
		
		set (BUILD_SHARED ON)

	else ()

		# All flags OFF, that's unusual but possible.
		# We assume the user wants only static libraries.

		set (BUILD_STATIC ON)

	endif ()
endif ()

# The main library

if (BUILD_STATIC)

	add_library                ("ssm-static" STATIC ${MAINLIB_SRC_FILES})
    target_include_directories ("ssm-static" PUBLIC ${MAINLIB_INCLUDE_DIRS})
    set_target_properties      ("ssm-static" PROPERTIES OUTPUT_NAME "ssm")

endif ()

if (BUILD_SHARED)
    
    add_library                ("ssm-shared" SHARED ${MAINLIB_SRC_FILES})
    target_include_directories ("ssm-shared" PUBLIC ${MAINLIB_INCLUDE_DIRS})
    set_target_properties      ("ssm-shared" PROPERTIES OUTPUT_NAME "ssm")
    
endif ()

# install (TARGETS ${SSM_MAINLIB_NAME}
         # ARCHIVE DESTINATION "static"
         # RUNTIME DESTINATION "

# Build the test executable, if applicable.

if (SSM_BUILD_TESTS)
    
    # Building tests when cross-compiling is unusual.
    # Tests might still work if the builder's architecture supports it,
    # or if the user manually copy & runs them on a target environment.
    
    if (CMAKE_CROSSCOMPILING)
        message (WARNING "Cross-compiling the test program. The test program will be built, but it might not run on the current machine.")
    endif ()

    # Extend CMAKE_PREFIX_PATH if required

    if (NOT SSM_BOOST_PATH STREQUAL "")
        list (APPEND CMAKE_PREFIX_PATH ${SSM_BOOST_PATH})
    endif ()

    # Force find_package to search again, at each run

    if (boost_headers_DIR)
        unset (boost_headers_DIR CACHE)
    endif ()
    
    find_package ("boost_headers" REQUIRED CONFIG)

    add_executable ("ssm-unit-test" # TODO: make static & shared executables (if applicable) to test both libs
                    "code/unit-test-01.cpp")
    
    target_link_libraries ("ssm-unit-test"
                           "Boost::headers")
endif ()

# testing, delete this asap

# add_executable ("ssm-dev-test"
                # "code/main.cpp"
				# "code/main/src/progid.cpp")

# target_include_directories ("ssm-dev-test" PUBLIC
                            # "code/main/include")

# if (WIN32)
	# add_or_replace_flag (CMAKE_CXX_FLAGS "/W[0-3]" "/W4")
# else ()
	# add_flag (CMAKE_CXX_FLAGS "-Wall")
	# add_flag (CMAKE_CXX_FLAGS "-Wextra")
# endif ()

##########################


# Build examples

set (EXAMPLE_SRC_FILES "example/example.cpp")

if (BUILD_STATIC)

    add_executable ("ssm-example-static" ${EXAMPLE_SRC_FILES})
    target_link_libraries ("ssm-example-static" "ssm-static")
    
endif ()

if (BUILD_SHARED)

    add_executable ("ssm-example" ${EXAMPLE_SRC_FILES})
    target_link_libraries ("ssm-example" "ssm-shared")

endif ()










# Give a configuration recap

function (bool_concat MSG VAL)
	if (${VAL})
		set (${MSG} "${${MSG}} yes" PARENT_SCOPE)
	else ()
		set (${MSG} "${${MSG}} no"  PARENT_SCOPE)
	endif ()
endfunction ()

set (STATUS_BUILD_STATIC "static libraries ... ")
set (STATUS_BUILD_SHARED "shared libraries ... ")
set (STATUS_BUILD_TESTS  "tests .............. ")

bool_concat (STATUS_BUILD_STATIC BUILD_STATIC)
bool_concat (STATUS_BUILD_SHARED BUILD_SHARED)
bool_concat (STATUS_BUILD_TESTS  SSM_BUILD_TESTS)

message (STATUS "
  Small State Machines - configuration recap:
  
  ${STATUS_BUILD_STATIC}
  ${STATUS_BUILD_SHARED}
  ${STATUS_BUILD_TESTS}
")
