cmake_minimum_required(VERSION 3.5)
project(xiao)

#option(BUILD_DOC "Build Doxygen documentation" OFF)
#option(BUILD_C-ARES "Build C-ARES" ON)
#option(BUILD_TESTING "Build tests" OFF)
#option(BUILD_SHARED_LIBS "Build trantor as a shared lib" OFF)
#option(XIAO_USE_TLS "TLS provider for xiao. Valid options are 'openssl', 'botan' or '' (let the build scripr decide)" "")
#option(USE_SPDLOG "Allow using the spdlog logging library" OFF)

#list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

set(XIAO_MAJOR_VERSION 0)
set(XIAO_MINOR_VERSION 0)
set(XIAO_PATCH_VERSION 1)
set(XIAO_VERSION
    ${XIAO_MAJOR_VERSION}.${XIAO_MINOR_VERSION}.${XIAO_PATCH_VERSION})

#include(GNUInstallDirs)
# Offer the user the choice of overriding the installation directories
#set(INSTALL_BIN_DIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for binaries")
#set(INSTALL_LIB_DIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
#set(INSTALL_INCLUDE_DIR
    #${CMAKE_INSTALL_INCLUDEDIR}
    #CACHE PATH "Installation directory for header files")
#set(DEF_INSTALL_XIAO_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/Xiao)
#set(INSTALL_XIAO_CMAKE_DIR
    #${DEF_INSTALL_XIAO_CMAKE_DIR}
    #CACHE PATH "Installation directory for cmake files")

add_library(${PROJECT_NAME})
#if(BUILD_SHARED_LIBS)
  #list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
 #           "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}" isSystemDir)
  #if("${isSystemDir}" STREQUAL "-1")
  #  set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}")
 # endif("${isSystemDir}" STREQUAL "-1")
 # set_target_properties(${PROJECT_NAME} PROPERTIES
    #  VERSION ${XIAO_VERSION}
    #  SOVERSION ${XIAO_MAJOR_VERSION})
 # if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    # Ignore MSVC C4251 and C4275 warning of exporting std objects with no dll export
    # We export class to facilitate maintenance, thus if you compile
    # drogon on windows as a shared library, you will need to use
    # exact same compiler for drogon and your app.
   # target_compile_options(${PROJECT_NAME} PUBLIC /wd4251 /wd4275)
  #endif()
#endif(BUILD_SHARED_LIBS)

# Tells Visual Studio 2017 (15.7+) and newer to correctly set the value of the standard __cplusplus macro,
# instead of leaving it to 199711L and settings the effective c++ version in _MSVC_LANG
# Dropping support for older versions of VS would allow to only rely on __cplusplus
#if(MSVC AND MSVC_VERSION GREATER_EQUAL 1914)
 # add_compile_options(/Zc:__cplusplus)
#endif(MSVC AND MSVC_VERSION GREATER_EQUAL 1914)

#if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" AND CMAKE_CXX_COMPILER_ID MATCHES Clang|GNU)
  #  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror)
#endif()

#if(${CMAKE_SYSTEM_NAME} STREQUAL "Haiku")
  #  target_link_libraries(${PROJECT_NAME} PRIVATE network)
#endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/exports/xiao/exports.h)

# include directories
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
         $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
  PRIVATE ${PROJECT_SOURCE_DIR}
          ${PROJECT_SOURCE_DIR}/xiao/utils
          ${PROJECT_SOURCE_DIR}/xiao/net
          ${PROJECT_SOURCE_DIR}/xiao/net/inner
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/third_party/wepoll>)

#if(MINGW)
 # target_compile_definitions(
  #  ${PROJECT_NAME}
 #   PUBLIC -D_WIN32_WINNT=0x0601)
#endif(MINGW)


set(XIAO_UTIL_SOURCES
    xiao/utils/AsyncFileLogger.cpp
    xiao/utils/LogStream.cpp
    xiao/utils/Date.cpp
    xiao/utils/Logger.cpp
    xiao/utils/Utilities.cpp
    xiao/utils/ConcurrentTaskQueue.cpp
    xiao/utils/MsgBuffer.cpp
    xiao/utils/TimingWheel.cpp
)
set(XIAO_NET_SOURCES
    xiao/net/EventLoop.cpp
)

set(XIAO_SOURCES

    #xiao/utils/SerialTaskQueue.cc

    #xiao/net/EventLoop.cc
    #xiao/net/EventLoopThread.cc
    #xiao/net/EventLoopThreadPool.cc
    #xiao/net/InetAddress.cc
    #xiao/net/TcpClient.cc
    #xiao/net/TcpServer.cc
    #xiao/net/Channel.cc
    #xiao/net/inner/Acceptor.cc
    #xiao/net/inner/Connector.cc
    #xiao/net/inner/Poller.cc
    #xiao/net/inner/Socket.cc
    #xiao/net/inner/MemBufferNode.cc
    #xiao/net/inner/StreamBufferNode.cc
    #xiao/net/inner/AsyncStreamBufferNode.cc
    #xiao/net/inner/TcpConnectionImpl.cc
    #xiao/net/inner/Timer.cc
    #xiao/net/inner/TimerQueue.cc
    #xiao/net/inner/poller/EpollPoller.cc
    #xiao/net/inner/poller/KQueue.cc
    #xiao/net/inner/poller/PollPoller.cc
    )
set(private_headers
    #xiao/net/inner/Acceptor.h
    #xiao/net/inner/Connector.h
    #xiao/net/inner/Poller.h
    #xiao/net/inner/Socket.h
    #xiao/net/inner/TcpConnectionImpl.h
    #xiao/net/inner/Timer.h
    #xiao/net/inner/TimerQueue.h
    #xiao/net/inner/poller/EpollPoller.h
    #xiao/net/inner/poller/KQueue.h
    #xiao/net/inner/poller/PollPoller.h
    )
#
if(WIN32)
  set(XIAO_SOURCES
#      ${XIAO_SOURCES}
     ${XIAO_UTIL_SOURCES}
      #third_party/wepoll/Wepoll.c
      xiao/utils/WindowsSupport.cpp
      #xiao/net/inner/FileBufferNodeWin.cc
      )
  set(private_headers
      ${private_headers}
      #third_party/wepoll/Wepoll.h
      xiao/utils/WindowsSupport.h
      )
else(WIN32)
    set(XIAO_SOURCES
        ${TRANTOR_SOURCES}
        #xiao/net/inner/FileBufferNodeUnix.cc
        )
endif(WIN32)

# Somehow the default value of TRANTOR_USE_TLS is OFF
#if(XIAO_USE_TLS STREQUAL OFF)
#  set(XIAO_USE_TLS "")
#endif()
#set(VALID_TLS_PROVIDERS "openssl" "botan" "none")
#list(FIND VALID_TLS_PROVIDERS "${TRANTOR_USE_TLS}" PREFERED_TLS_IDX)
#if(PREFERED_TLS_IDX EQUAL -1 AND NOT XIAO_USE_TLS STREQUAL "")
#  message(FATAL_ERROR "Invalid TLS provider: ${TRANTOR_USE_TLS}\n"
#                      "Valid TLS providers are: ${VALID_TLS_PROVIDERS}")
#endif()
#
#set(XIAO_TLS_PROVIDER "None")
#if(XIAO_USE_TLS STREQUAL "openssl" OR XIAO_USE_TLS STREQUAL "")
#  find_package(OpenSSL)
#  if(OpenSSL_FOUND)
#    target_link_libraries(${PROJECT_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_OPENSSL)
#    set(XIAO_TLS_PROVIDER "OpenSSL")
#
#    set(XIAO_SOURCES
#      ${XIAO_SOURCES}
#      xiao/net/inner/tlsprovider/OpenSSLProvider.cc
#      xiao/utils/crypto/openssl.cc)
#  elseif(XIAO_USE_TLS STREQUAL "openssl")
#    message(FATAL_ERROR "Requested OpenSSL TLS provider but OpenSSL was not found")
#  endif()
#endif()

#if(XIAO_TLS_PROVIDER STREQUAL "None"
#  AND (XIAO_USE_TLS STREQUAL "botan" OR XIAO_USE_TLS STREQUAL ""))
#  find_package(Botan)
#  if(Botan_FOUND)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE USE_BOTAN)
#    target_link_libraries(${PROJECT_NAME} PRIVATE Botan::Botan)
#    set(XIAO_TLS_PROVIDER "Botan")
#
#    set(XIAO_SOURCES
#        ${XIAO_SOURCES}
#        #xiao/net/inner/tlsprovider/BotanTLSProvider.cc
#        #xiao/utils/crypto/botan.cc
#        )
#  elseif(XIAO_USE_TLS STREQUAL "botan")
#    message(FATAL_ERROR "Requested Botan TLS provider but Botan was not found")
#  endif()
#endif()
#
#if(XIAO_TLS_PROVIDER STREQUAL "None")
#  set(XIAO_SOURCES
#      ${XIAO_SOURCES}
#      #xiao/utils/crypto/sha3.cc
#      #xiao/utils/crypto/md5.cc
#      #xiao/utils/crypto/sha1.cc
#      #xiao/utils/crypto/sha256.cc
#      #xiao/utils/crypto/blake2.cc
#      )
#  set(private_headers
#      ${private_headers}
#      #xiao/utils/crypto/sha3.h
#      #xiao/utils/crypto/md5.h
#      #xiao/utils/crypto/sha1.h
#      #xiao/utils/crypto/sha256.h
#      )
#endif()
#
#message(STATUS "Xiao using SSL library: ${XIAO_TLS_PROVIDER}")
#target_compile_definitions(${PROJECT_NAME} PRIVATE XIAO_TLS_PROVIDER=${XIAO_TLS_PROVIDER})
#
#set(HAVE_SPDLOG NO)
#if(USE_SPDLOG)
#  find_package(spdlog CONFIG)
#  if(spdlog_FOUND)
#    message(STATUS "spdlog found!")
#    set(HAVE_SPDLOG TRUE)
#  endif(spdlog_FOUND)
#endif(USE_SPDLOG)
#if(HAVE_SPDLOG)
#  target_link_libraries(${PROJECT_NAME} PUBLIC spdlog::spdlog_header_only)
#  target_compile_definitions(${PROJECT_NAME} PUBLIC TRANTOR_SPDLOG_SUPPORT SPDLOG_FMT_EXTERNAL FMT_HEADER_ONLY)
#endif(HAVE_SPDLOG)
#
#set(HAVE_C-ARES NO)
#if (BUILD_C-ARES)
#    find_package(c-ares)
#    if(c-ares_FOUND)
#      message(STATUS "c-ares found!")
#      set(HAVE_C-ARES TRUE)
#    endif()
#endif ()
#
#if(HAVE_C-ARES)
#  if (NOT BUILD_SHARED_LIBS)
#    target_compile_definitions(${PROJECT_NAME} PRIVATE CARES_STATICLIB)
#  endif ()
#  target_link_libraries(${PROJECT_NAME} PRIVATE c-ares_lib)
#  set(XIAO_SOURCES
#      ${XIAO_SOURCES}
#      xiao/net/inner/AresResolver.cc)
#  set(private_headers
#      ${private_headers}
#      xiao/net/inner/AresResolver.h)
#  if(APPLE)
#    target_link_libraries(${PROJECT_NAME} PRIVATE resolv)
#  elseif(WIN32)
#    target_link_libraries(${PROJECT_NAME} PRIVATE iphlpapi)
#  endif()
#else()
#  set(XIAO_SOURCES
#      ${XIAO_SOURCES}
#      xiao/net/inner/NormalResolver.cc)
#  set(private_headers
#      ${private_headers}
#      xiao/net/inner/NormalResolver.h)
#endif()
#
#find_package(Threads)
#target_link_libraries(${PROJECT_NAME} PUBLIC Threads::Threads)
#if(WIN32)
#  target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32 rpcrt4)
#  if(OpenSSL_FOUND)
#    target_link_libraries(${PROJECT_NAME} PRIVATE crypt32 secur32)
#  endif(OpenSSL_FOUND)
#else(WIN32)
#  target_link_libraries(${PROJECT_NAME} PRIVATE pthread $<$<PLATFORM_ID:SunOS>:socket>)
#endif(WIN32)
#
#file(WRITE ${CMAKE_BINARY_DIR}/test_atomic.cpp
#     "#include <atomic>\n"
#     "int main() { std::atomic<int64_t> i(0); i++; return 0; }\n")
#try_compile(ATOMIC_WITHOUT_LINKING ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/test_atomic.cpp)
#if (NOT ATOMIC_WITHOUT_LINKING)
#    target_link_libraries(${PROJECT_NAME} PUBLIC atomic)
#endif ()
#file(REMOVE ${CMAKE_BINARY_DIR}/test_atomic.cpp)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME Xiao)

#if(BUILD_TESTING)
#  add_subdirectory(xiao/tests)
#  find_package(GTest)
#  if(GTest_FOUND)
#    enable_testing()
#    add_subdirectory(xiao/unittests)
#  endif()
#endif()

set(public_net_headers
    #xiao/net/EventLoop.h
    #xiao/net/EventLoopThread.h
    #xiao/net/EventLoopThreadPool.h
    #xiao/net/InetAddress.h
    #xiao/net/TcpClient.h
    #xiao/net/TcpConnection.h
    #xiao/net/TcpServer.h
    xiao/net/AsyncStream.h
    #xiao/net/callbacks.h
    #xiao/net/Resolver.h
    #xiao/net/Channel.h
    #xiao/net/Certificate.h
    #xiao/net/TLSPolxiao
    )
set(public_utils_headers
    xiao/utils/AsyncFileLogger.h
    xiao/utils/ConcurrentTaskQueue.h
    xiao/utils/Date.h
    xiao/utils/Funcs.h
    xiao/utils/LockFreeQueue.h
    xiao/utils/xiao_marco.h
    xiao/utils/LogStream.h
    xiao/utils/Logger.h
    xiao/utils/MsgBuffer.h
    xiao/utils/NonCopyable.h
    xiao/utils/ObjectPool.h
    #xiao/utils/SerialTaskQueue.h
    xiao/utils/TaskQueue.h
    xiao/utils/TimingWheel.h
    xiao/utils/Utilities.h
    )

target_sources(${PROJECT_NAME} PRIVATE
               ${XIAO_SOURCES}
               ${XIAO_NET_SOURCES}
               ${CMAKE_CURRENT_BINARY_DIR}/exports/xiao/exports.h
               ${public_net_headers}
               ${public_utils_headers}
               ${private_headers})

source_group("Util"
            FILES
            ${XIAO_UTIL_SOURCES}
)       

source_group("Net"
            FILES
            ${XIAO_NET_SOURCES}
)       

source_group("Public API"
             FILES
             ${CMAKE_CURRENT_BINARY_DIR}/exports/xiao/exports.h
             ${public_net_headers}
             ${public_utils_headers})

source_group("Private Headers"
             FILES
             ${private_headers})

#install(TARGETS trantor
#                # IMPORTANT: Add the trantor library to the "export-set"
#        EXPORT TrantorTargets
#        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
#        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
#        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib)
#install(FILES ${CMAKE_CURRENT_BINARY_DIR}/exports/trantor/exports.h
#        DESTINATION ${INSTALL_INCLUDE_DIR}/trantor)
#install(FILES ${public_net_headers}
#        DESTINATION ${INSTALL_INCLUDE_DIR}/trantor/net)
#install(FILES ${public_utils_headers}
#        DESTINATION ${INSTALL_INCLUDE_DIR}/trantor/utils)

#include(CMakePackageConfigHelpers)
## ... for the install tree
#configure_package_config_file(
#  cmake/templates/TrantorConfig.cmake.in
#  ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/TrantorConfig.cmake
#  INSTALL_DESTINATION
#  ${INSTALL_TRANTOR_CMAKE_DIR})

# version
#write_basic_package_version_file(
#  ${CMAKE_CURRENT_BINARY_DIR}/TrantorConfigVersion.cmake
#  VERSION ${TRANTOR_VERSION}
#  COMPATIBILITY SameMajorVersion)

# Install the TrantorConfig.cmake and TrantorConfigVersion.cmake
#install(
#  FILES
#    "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/TrantorConfig.cmake"
#    "${CMAKE_CURRENT_BINARY_DIR}/TrantorConfigVersion.cmake"
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/Findc-ares.cmake"
#    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/FindBotan.cmake"
#  DESTINATION "${INSTALL_TRANTOR_CMAKE_DIR}"
#  COMPONENT dev)

# Install the export set for use with the install-tree
#install(EXPORT TrantorTargets
#        DESTINATION "${INSTALL_TRANTOR_CMAKE_DIR}"
#        NAMESPACE Trantor::
#        COMPONENT dev)

# Doxygen documentation
#find_package(Doxygen OPTIONAL_COMPONENTS dot dia)
#if(DOXYGEN_FOUND)
#  set(DOXYGEN_PROJECT_BRIEF "Non-blocking I/O cross-platform TCP network library, using C++14")
#  set(DOXYGEN_OUTPUT_DIRECTORY docs/${PROJECT_NAME})
#  set(DOXYGEN_GENERATE_LATEX NO)
#  set(DOXYGEN_BUILTIN_STL_SUPPORT YES)
#  set(DOXYGEN_USE_MDFILE_AS_MAINPAGE README.md)
#  set(DOXYGEN_STRIP_FROM_INC_PATH ${PROJECT_SOURCE_DIR}
#                                  ${CMAKE_CURRENT_BINARY_DIR}/exports)
#  if (WIN32)
#    set(DOXYGEN_PREDEFINED _WIN32)
#  endif(WIN32)
#  doxygen_add_docs(doc_${PROJECT_NAME}
#                   README.md
#                   ChangeLog.md
#                   ${public_net_headers}
#                   ${public_utils_headers}
#                   COMMENT "Generate documentation")
#  if(NOT TARGET doc)
#    add_custom_target(doc)
#  endif()
#  add_dependencies(doc doc_${PROJECT_NAME})
#  if (BUILD_DOC)
#    add_dependencies(${PROJECT_NAME} doc_${PROJECT_NAME})
#    # Don't install twice, so limit to Debug (assume developer)
#    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs/${PROJECT_NAME}
#            TYPE DOC
#            CONFIGURATIONS Debug)
#  endif(BUILD_DOC)
#endif(DOXYGEN_FOUND)

