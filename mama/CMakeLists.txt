project (mama)

enable_testing()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/c_cpp/cmake)
include(mama-common)

# Common flag setup
if(WIN32)
	add_definitions(-DMAMA_DLL)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libc.lib")
endif()
add_definitions(-DOPENMAMA_INTEGRATION)

get_component_version(MAMA)

add_subdirectory (c_cpp/src/c)
add_subdirectory (c_cpp/src/c/dqstrategyplugin)

if (WITH_JAVA)
    add_subdirectory (jni/src/c)
endif ()

if(WITH_CPP)
    add_subdirectory (c_cpp/src/cpp)
endif()

if (WITH_EXAMPLES)
    add_subdirectory (c_cpp/src/examples/c)
    if(WITH_CPP)
        add_subdirectory (c_cpp/src/examples/cpp)
    endif()
endif ()

if (WITH_TESTTOOLS)
    add_subdirectory (c_cpp/src/testtools/capturereplay/c)
    add_subdirectory (c_cpp/src/testtools/load/c)
    if(WITH_CPP)
        add_subdirectory (c_cpp/src/testtools/load/cpp)
    endif()
    if (NOT WIN32)
        add_subdirectory (c_cpp/src/testtools/performance/c)
        if(WITH_CPP)
            add_subdirectory (c_cpp/src/testtools/performance/cpp)
        endif()
    endif ()
endif ()

if (WITH_UNITTEST)
    add_subdirectory (c_cpp/src/gunittest/c)
    if(WITH_CPP)
        add_subdirectory (c_cpp/src/gunittest/cpp)
    endif()
endif ()

if (WITH_BASE_BRIDGE)
    add_subdirectory (c_cpp/src/c/bridge/base)
endif ()

if (WITH_NOOP_BRIDGE)
    add_subdirectory (c_cpp/src/c/bridge/noop)
endif ()

if (WITH_OMPB_BRIDGE)
    add_subdirectory (c_cpp/src/c/bridge/ompb)
endif ()

if (WITH_PROTON)
    add_subdirectory (c_cpp/src/c/bridge/qpid)
    add_subdirectory (c_cpp/src/c/payload/qpidmsg)
endif ()
