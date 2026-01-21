
#include(ExternalProject)
#
#ExternalProject_Add(
#    NakamaSDK
#    GIT_REPOSITORY https://github.com/heroiclabs/nakama-cpp.git
#    GIT_TAG master
#    PREFIX ${CMAKE_BINARY_DIR}/nakama-sdk
#    SOURCE_DIR ${CMAKE_BINARY_DIR}/nakama-sdk/src
#    BINARY_DIR ${CMAKE_BINARY_DIR}/nakama-sdk/build
#
#    # Configure using the preset
#    CONFIGURE_COMMAND cmake --preset linux-amd64 --source ${CMAKE_BINARY_DIR}/nakama-sdk/src
#
#    # Build using preset output, install to the default location
#    BUILD_COMMAND cmake --build . --config MinSizeRel --target install
#
#    INSTALL_COMMAND ""  # install is handled by the build target
#)
#
#
## Location of Nakama install
#set(NAKAMA_INSTALL_DIR ${CMAKE_BINARY_DIR}/nakama-sdk/build/linux-amd64/install)
#set(NAKAMA_INCLUDE_DIR ${NAKAMA_INSTALL_DIR}/include)
#set(NAKAMA_LIB_DIR ${NAKAMA_INSTALL_DIR}/lib)
#
## Create imported library target
#if(WIN32)
#    set(NAKAMA_LIB_FILE "${NAKAMA_LIB_DIR}/nakama-cpp.lib")
#else()
#    set(NAKAMA_LIB_FILE "${NAKAMA_LIB_DIR}/libnakama-cpp.a")
#endif()
#
#add_custom_target(build_nakama ALL
#    DEPENDS NakamaSDK
#)
#
#add_library(nakama_cpp STATIC IMPORTED GLOBAL)
#add_dependencies(nakama_cpp build_nakama)







#ExternalProject_Add_StepTargets(NakamaSDK build)
#
#add_library(nakama-cpp STATIC IMPORTED GLOBAL)
#add_dependencies(nakama-cpp NakamaSDK-build)
#set_target_properties(nakama-cpp PROPERTIES
#    IMPORTED_LOCATION "${NAKAMA_LIB_FILE}"
#    INTERFACE_INCLUDE_DIRECTORIES "${NAKAMA_INCLUDE_DIR}"
#)













#set(NAKAMA_USE_VCPKG ON CACHE BOOL "" FORCE)


## Depends on windows / linux etc
#set(WITH_HTTP_CURL ON CACHE BOOL "" FORCE)
#set(WITH_WS_WSLAY ON CACHE BOOL "" FORCE)
#set(CFG_CURL_SYSTEM ON CACHE BOOL "" FORCE)

##set(CMAKE_SYSTEM_PROCESSOR AMD64 CACHE STRING "" FORCE)
##set(VCPKG_TARGET_TRIPLET x64-windows-static-heroic CACHE STRING "" FORCE)
##set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE ${sourceDir}/cmake/toolchains/vcpkg-msvc.cmake CACHE STRING "" FORCE)
##set(WITH_WS_LIBHTTPC ON CACHE BOOL "" FORCE)
##set(WITH_HTTP_LIBHTTPC ON CACHE BOOL "" FORCE)
##set(BUILD_TESTING ON CACHE BOOL "" FORCE)

#if(NOT DEFINED CMAKE_TOOLCHAIN_FILE OR
#        NOT CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg")
#    message(FATAL_ERROR "Vcpkg toolchain not detected! Make sure to pass -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake")
#endif()

#find_package(RapidJSON CONFIG REQUIRED)
#find_package(Protobuf CONFIG REQUIRED)
##find_package(wslay CONFIG REQUIRED)

#FetchContent_Declare(
#        wslay
#        GIT_REPOSITORY https://github.com/tatsuhiro-t/wslay.git
#        GIT_TAG master
#        GIT_PROGRESS   TRUE
#)
#FetchContent_MakeAvailable(wslay)

##FetchContent_Declare(
##        protobuf
##        GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
##        GIT_TAG v3.21.12
##        GIT_PROGRESS   TRUE
##)
##FetchContent_MakeAvailable(protobuf)


#FetchContent_Declare(
#    nakama-sdk
#    GIT_REPOSITORY https://github.com/haraldwer/nakama-cpp-cmake.git
#    GIT_TAG master
#    GIT_SUBMODULES ""  # important
#    GIT_PROGRESS   TRUE
#)
#FetchContent_MakeAvailable(nakama-sdk)