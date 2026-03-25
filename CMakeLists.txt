cmake_minimum_required(VERSION 3.16)
project(RingFTPClient LANGUAGES C)

include(FetchContent)

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

# Detect musl libc
option(MUSL "Build for musl libc" OFF)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	# Auto-detect musl if not explicitly set
	if(NOT MUSL)
		execute_process(
			COMMAND ldd --version
			OUTPUT_VARIABLE LDD_OUTPUT
			ERROR_VARIABLE LDD_OUTPUT
			OUTPUT_STRIP_TRAILING_WHITESPACE
		)
		if(LDD_OUTPUT MATCHES "musl")
			set(MUSL ON)
			message(STATUS "Detected musl libc")
		endif()
	endif()
endif()

# Set the destination directory for the built library
if(MUSL AND OS_DIR STREQUAL "linux")
	set(LIB_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/musl/${ARCH_DIR}")
else()
	set(LIB_DEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${OS_DIR}/${ARCH_DIR}")
endif()

# Build wolfSSL and curl from source statically
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Fetch and build wolfSSL
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(WOLFSSL_OPENSSLEXTRA ON CACHE BOOL "Enable OpenSSL Extra" FORCE)
set(WOLFSSL_ASM OFF CACHE BOOL "Enable ASM" FORCE)
set(WOLFSSL_EXAMPLES OFF CACHE BOOL "Build Examples" FORCE)
set(WOLFSSL_CRYPT_TESTS OFF CACHE BOOL "Build tests" FORCE)
set(WOLFSSL_CURL ON CACHE BOOL "Enable curl support" FORCE)
set(WOLFSSL_SYS_CA_CERTS ON CACHE BOOL "Enable system CA certs" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/wolfssl/CMakeLists.txt")
    message(STATUS "Adding wolfSSL submodule...")
    if(NOT MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
    endif()
    add_subdirectory(src/c_src/wolfssl)
    set(WOLFSSL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/wolfssl" CACHE PATH "" FORCE)
else()
    message(STATUS "Fetching wolfSSL...")
    if(NOT MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-error")
    endif()
    FetchContent_Declare(wolfssl
        GIT_REPOSITORY https://github.com/wolfSSL/wolfssl.git
        GIT_TAG        v5.9.0-stable
    )
    FetchContent_MakeAvailable(wolfssl)
    FetchContent_GetProperties(wolfssl SOURCE_DIR WOLFSSL_SOURCE_DIR)
    set(WOLFSSL_INCLUDE_DIR "${WOLFSSL_SOURCE_DIR}" CACHE PATH "" FORCE)
endif()

# Since curl expects find_package(WolfSSL) to succeed, let's mock it
set(WOLFSSL_LIBRARY wolfssl CACHE STRING "" FORCE)

# Fetch and build cURL
set(BUILD_CURL_EXE OFF CACHE BOOL "Build curl executable" FORCE)
set(CURL_USE_LIBPSL OFF CACHE BOOL "Use libpsl" FORCE)
set(CURL_USE_LIBSSH2 OFF CACHE BOOL "Use libssh2" FORCE)
set(CURL_USE_LIBSSH OFF CACHE BOOL "Use libssh" FORCE)
set(USE_LIBIDN2 OFF CACHE BOOL "Use libidn2" FORCE)
set(CURL_DISABLE_LDAP ON CACHE BOOL "Disable ldap" FORCE)
set(CURL_DISABLE_LDAPS ON CACHE BOOL "Disable ldaps" FORCE)
set(CURL_USE_WOLFSSL ON CACHE BOOL "Enable wolfSSL" FORCE)
set(CURL_USE_OPENSSL OFF CACHE BOOL "Enable OpenSSL" FORCE)
set(CURL_USE_SCHANNEL OFF CACHE BOOL "Enable Schannel" FORCE)
set(CURL_USE_SECTRANSP OFF CACHE BOOL "Enable Secure Transport" FORCE)
set(CURL_ZLIB OFF CACHE BOOL "Enable zlib" FORCE)
set(CURL_BROTLI OFF CACHE BOOL "Enable brotli" FORCE)
set(CURL_ZSTD OFF CACHE BOOL "Enable zstd" FORCE)
set(USE_NGHTTP2 OFF CACHE BOOL "Enable nghttp2" FORCE)
set(USE_NGTCP2 OFF CACHE BOOL "Enable ngtcp2" FORCE)
set(USE_QUICHE OFF CACHE BOOL "Enable quiche" FORCE)
set(CURL_DISABLE_INSTALL ON CACHE BOOL "Disable curl install" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "Disable tests" FORCE)
set(CURL_CA_NATIVE ON CACHE BOOL "Use native CA store" FORCE)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/curl/CMakeLists.txt")
    message(STATUS "Adding cURL submodule...")
    add_subdirectory(src/c_src/curl)
else()
    message(STATUS "Fetching cURL...")
    FetchContent_Declare(curl
        GIT_REPOSITORY https://github.com/curl/curl.git
        GIT_TAG        curl-8_19_0
    )
    FetchContent_MakeAvailable(curl)
endif()

if(TARGET libcurl_static)
    set(CURL_INCLUDE_DIRS "${curl_SOURCE_DIR}/include" "${curl_BINARY_DIR}/include/curl")
else()
    find_package(CURL REQUIRED)
endif()

# Create the Ring FTP Client extension shared library
add_library(ring_ftp SHARED
	${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/ring_ftp.c
)

# Include directories
target_include_directories(ring_ftp PRIVATE
	${RING_INCLUDE}
    ${CURL_INCLUDE_DIRS}
)

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/ftpclient/ftpclient.h")
    target_include_directories(ring_ftp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src/c_src/ftpclient)
else()
    # Fetch ftpclient.h if it doesn't exist locally
    message(STATUS "ftpclient.h source not found locally, fetching from GitHub...")
    FetchContent_Declare(
        ftpclient
        GIT_REPOSITORY https://github.com/ysdragon/ftpclient.h
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(ftpclient)
    FetchContent_GetProperties(ftpclient SOURCE_DIR FTPCLIENT_SOURCE_DIR)
    target_include_directories(ring_ftp PRIVATE ${FTPCLIENT_SOURCE_DIR})
endif()

# Link libraries
if(TARGET libcurl_static)
    set(RING_FTP_LIBS libcurl_static wolfssl)
elseif(TARGET libcurl)
    set(RING_FTP_LIBS libcurl wolfssl)
else()
    set(RING_FTP_LIBS curl wolfssl)
endif()

target_link_libraries(ring_ftp PRIVATE
	Ring::Ring
    ${RING_FTP_LIBS}
)

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
