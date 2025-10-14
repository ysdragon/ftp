cmake_minimum_required(VERSION 3.16)
project(RingFTPClient LANGUAGES C)

set(RING_ROOT_DEFAULT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
if(DEFINED ENV{RING})
    set(RING_ROOT_DEFAULT "$ENV{RING}")
endif()
set(RING_ROOT "${RING_ROOT_DEFAULT}" CACHE PATH "Path to the Ring project root directory.")
message(STATUS "Using Ring from: ${RING_ROOT}")

if(NOT TARGET Ring::Ring)
    add_library(Ring::Ring UNKNOWN IMPORTED)
    find_path(RING_INCLUDE_DIR ring.h PATHS "${RING_ROOT}/language/include")
    find_library(RING_LIBRARY_PATH NAMES ring PATHS "${RING_ROOT}/lib")

    if(NOT RING_INCLUDE_DIR OR NOT RING_LIBRARY_PATH)
        message(FATAL_ERROR "Could not find Ring headers or library in ${RING_ROOT}. "
                            "Please set the RING_ROOT cache variable correctly.")
    endif()

    set_target_properties(Ring::Ring PROPERTIES
        IMPORTED_LOCATION "${RING_LIBRARY_PATH}"
        INTERFACE_INCLUDE_DIRECTORIES "${RING_INCLUDE_DIR}"
    )
endif()

# Set paths
set(RING_INCLUDE "${RING_ROOT}/language/include")
set(RING_LIB "${RING_ROOT}/lib")
set(RING_BIN "${RING_ROOT}/bin")

# Determine OS and Architecture specific paths
string(TOLOWER "${CMAKE_SYSTEM_NAME}" OS_DIR)
if(OS_DIR STREQUAL "darwin")
    set(OS_DIR "macos")
endif()

string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" ARCH_DIR_RAW)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows" AND DEFINED CMAKE_GENERATOR_PLATFORM)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(ARCH_DIR_RAW "i386")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64")
        set(ARCH_DIR_RAW "amd64")
    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        set(ARCH_DIR_RAW "arm64")
    else()
        message(WARNING "Unsupported CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}, falling back to CMAKE_SYSTEM_PROCESSOR")
    endif()
endif()

if(NOT DEFINED ARCH_DIR)
    if(ARCH_DIR_RAW MATCHES "x86_64|amd64")
        set(ARCH_DIR "amd64")
    elseif(ARCH_DIR_RAW MATCHES "aarch64|arm64")
        set(ARCH_DIR "arm64")
    elseif(ARCH_DIR_RAW MATCHES "riscv64")
        set(ARCH_DIR "riscv64")
    elseif(ARCH_DIR_RAW MATCHES "i386|i686|x8x")
        set(ARCH_DIR "i386")
    else()
        set(ARCH_DIR "${ARCH_DIR_RAW}")
        message(WARNING "Unsupported architecture: ${ARCH_DIR_RAW}. Using as directory name.")
    endif()
endif()

# Set the destination directory for the built library
set(LIB_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")

# Find CURL package
find_package(CURL REQUIRED)

# Create the Ring FTP Client extension shared library
add_library(ring_ftp SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/ring_ftp.c
)

# Include directories
target_include_directories(ring_ftp PRIVATE
	${RING_INCLUDE}
	${CMAKE_CURRENT_SOURCE_DIR}
	${CURL_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(ring_ftp PRIVATE
	Ring::Ring
	CURL::libcurl
)

# On Windows, link additional dependencies required for static CURL
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	target_link_libraries(ring_ftp PRIVATE
		ws2_32
		wldap32
		crypt32
		normaliz
	)
endif()

target_compile_options(ring_ftp PRIVATE
    $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:
        $<$<C_COMPILER_ID:MSVC>:/O2>
        $<$<NOT:$<C_COMPILER_ID:MSVC>>:-O3>
        -DNDEBUG
    >
)

# Set target properties for output name and prefix
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	set_target_properties(ring_ftp PROPERTIES
		OUTPUT_NAME "ring_ftp"
	)
endif()

# Move built library to lib directory
add_custom_command(
	TARGET ring_ftp
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_DEST_DIR}
	COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:ring_ftp> ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_ftp>
	COMMENT "Moving built library to ${LIB_DEST_DIR}"
	VERBATIM
)

# Copy to bin directory on Windows
if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
	add_custom_command(
		TARGET ring_ftp
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E make_directory ${RING_BIN}
		COMMAND ${CMAKE_COMMAND} -E copy ${LIB_DEST_DIR}/$<TARGET_FILE_NAME:ring_ftp> ${RING_BIN}/$<TARGET_FILE_NAME:ring_ftp>
		COMMENT "Copying built library to bin directory"
		VERBATIM
	)
endif()

install(TARGETS ring_ftp
    LIBRARY DESTINATION "${RING_ROOT}/lib"
)

message(STATUS "Ring FTP Extension Configuration:")
message(STATUS "  - Ring Include Dir: ${RING_INCLUDE_DIR}")
message(STATUS "  - Ring Library Path: ${RING_LIBRARY_PATH}")