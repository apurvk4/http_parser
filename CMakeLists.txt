cmake_minimum_required(VERSION 3.2 FATAL_ERROR)

project("http_parser" CXX)

message(STATUS "[${PROJECT_NAME}] Processing ${CMAKE_CURRENT_LIST_FILE}")

# Force the use of Clang if available
find_program(CLANG_CPP NAMES clang++ clang++-10 clang++-9 clang++-8 clang++-7 clang++-6 clang++-5)

if(CLANG_CPP)
    set(CMAKE_CXX_COMPILER ${CLANG_CPP})
    message(STATUS "Clang C++ compiler found: ${CLANG_CPP}")
else()
    message(STATUS "Clang C++ compiler not found, using default C++ compiler")
endif()

# use the directory of this cmake file to set the source and include directories
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")

message(STATUS "Source directory in lib: ${SRC_DIR}")
message(STATUS "Include directory in lib: ${INCLUDE_DIR}")

file(GLOB_RECURSE SRC_FILES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE INCLUDE_FILES "${INCLUDE_DIR}/*.hpp" "${INCLUDE_DIR}/*.h")
file(GLOB_RECURSE LIBS_INCLUDE_FILES "${LIB_DIR}/*.hpp" "${LIB_DIR}/*.h")

# Collect all subdirectories under INCLUDE_DIR and LIB_DIR
file(GLOB_RECURSE ALL_INCLUDE_DIRS LIST_DIRECTORIES true "${INCLUDE_DIR}/*")
file(GLOB_RECURSE ALL_LIBS_INCLUDE_DIRS LIST_DIRECTORIES true "${LIB_DIR}/*")

# Filter only directories
set(DIR_LIST "")
foreach(DIR ${ALL_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND DIR_LIST ${DIR})
    endif()
endforeach()

foreach(DIR ${ALL_LIBS_INCLUDE_DIRS})
    if(IS_DIRECTORY ${DIR})
        list(APPEND DIR_LIST ${DIR})
    endif()
endforeach()

include_directories(${DIR_LIST})

# build http_parser as a dynamic library
add_library(${PROJECT_NAME} SHARED ${SRC_FILES} ${INCLUDE_FILES} ${LIBS_INCLUDE_FILES})

# Define PARSER_EXPORTS when building the library
target_compile_definitions(${PROJECT_NAME} PRIVATE PARSER_EXPORTS)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${DIR_LIST}
    ${INCLUDE_DIR}
    ${LIB_DIR}
)

add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

# Find packages
find_package(Threads REQUIRED)

# Link against libraries using keywords
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

# link against windows socket library if on windows
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 Mswsock AdvApi32)
endif()

# Set C++ standard to 17
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS NO
)

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Set debug flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
)

# set source groups
foreach(FILE ${SRC_FILES} ${INCLUDE_FILES} ${LIBS_INCLUDE_FILES})
    get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)

    # Remove common directory prefix to make the group
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" GROUP "${PARENT_DIR}")

    # Make sure we are using windows slashes
    string(REPLACE "/" "\\" GROUP "${GROUP}")

    source_group("${GROUP}" FILES "${FILE}")
endforeach()

# Enable multiprocessing for MSVC
if (MSVC)
    target_compile_options(${PROJECT_NAME}
        PRIVATE "/MP"
    )
endif()
