cmake_minimum_required(VERSION 2.8.1)
project(tydns)

LIST(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")


###### BEGIN FLAGS

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -Wunused-function")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11 -Wunused-function")

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    if (NOT ENABLE_SEC)
        message(STATUS "CANARY , PIE , RELRO , FORTIFY not enabled, if you have enough performance (laptop/pc/server) / memory (>2MB) , and want to protect against pwning  , please enable it by -DENABLE_SEC=1")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
    else()
        if (WIN32)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -g -pipe -Wall -Wp,-D_FORTIFY_SOURCE=2 -fexceptions --param=ssp-buffer-size=4")
        else()
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fstack-protector-all -D_FORTIFY_SOURCE=2 -O2")
            if (APPLE)
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE -Wl,-pie")
            else()
                set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-z,relro,-z,now -fPIE -pie")
            endif()
        endif()
    endif()
endif()

if (WIN32)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWINVER=0x0600 -D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWINVER=0x0600 -D_WIN32_WINNT=0x0600 -DNTDDI_VERSION=0x06000000")
endif()

set(CMAKE_C_FLAGS_DEBUG "-DDEBUG -g")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG -g")

####### END FLAGS



####### BEGIN LIBS

find_package(Tcmalloc)
find_package(Threads)

if (NOT CMAKE_CROSSCOMPILING)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

# Here we used a hack, because linking to .a in CMake did something redunant to our process
# Just '-static' can make all libraries statically linked into executable
# Both statically and dynamically linking use simple arguments like '-lz'
# So we find a .so, but using a .a in fact
# A clean way: see http://cmake.3232098.n2.nabble.com/Howto-compile-static-executable-tp5580269p5582967.html
if (BUILD_STATIC)
    if (APPLE)
        SET(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})
    else()
        set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
    endif()
endif()

if (CMAKE_CROSSCOMPILING AND APPLE)
    find_package(ZLIB REQUIRED)
endif()
if (NOT APPLE)
find_package(Threads REQUIRED)
endif()

if (WIN32)
    set(SYSTEM_LIBRARY ws2_32 pthread winpthread)
else()
    set(SYSTEM_LIBRARY dl c)
endif()

include_directories(${Boost_INCLUDE_DIR})

file(GLOB SOURCE_FILES
    "src/*.h"
    "src/*.c"
    "src/*.hpp"
    "src/*.cpp"
)

add_executable(${PROJECT_NAME}d ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}d ${CMAKE_THREAD_LIBS_INIT} m ${SYSTEM_LIBRARY})
if (Tcmalloc_FOUND)
    target_link_libraries(${PROJECT_NAME}d ${Tcmalloc_LIBRARIES})
endif(Tcmalloc_FOUND)



