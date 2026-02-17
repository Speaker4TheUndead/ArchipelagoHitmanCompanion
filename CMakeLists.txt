cmake_minimum_required(VERSION 3.15)

project(ArchipelagoHitmanCompanion CXX)

# Find latest version at https://github.com/OrfeasZ/ZHMModSDK/releases
# Set ZHMMODSDK_DIR variable to a local directory to use a local copy of the ZHMModSDK.
set(ZHMMODSDK_VER "v4.0.0-rc.3")
include(cmake/setup-zhmmodsdk.cmake)

include(FetchContent)
FetchContent_Declare(
        asio
        URL https://github.com/chriskohlhoff/asio/archive/f693a3eb7fe72a5f19b975289afc4f437d373d9c.zip)
FetchContent_Declare(
        json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_Declare(
        websocketpp
        URL https://github.com/zaphoyd/websocketpp/archive/4dfe1be74e684acca19ac1cf96cce0df9eac2a2d.zip)
FetchContent_Declare(
        wswrap
        GIT_REPOSITORY https://github.com/black-sliver/wswrap.git
        GIT_TAG d0505e0ec53a26743f11051949a0dc66bcf44951)

FetchContent_MakeAvailable(asio json websocketpp wswrap)


# Set C++ standard to C++23.
set(CMAKE_CXX_STANDARD 23)

# Create the ArchipelagoHitmanCompanion mod library.
add_library(ArchipelagoHitmanCompanion SHARED
    src/ArchipelagoHitmanCompanion.cpp
    src/ArchipelagoHitmanCompanion.h
)

target_compile_options(ArchipelagoHitmanCompanion PRIVATE
    $<IF:$<OR:$<CXX_COMPILER_ID:MSVC>,$<C_COMPILER_ID:MSVC>>,/bigobj,"">
)

# find OpenSSL and enable SSL support if found
find_package(OpenSSL)
if(OpenSSL_FOUND)
    target_link_libraries(ArchipelagoHitmanCompanion
            PRIVATE OpenSSL::SSL OpenSSL::Crypto)
else()
    target_compile_definitions(ArchipelagoHitmanCompanion
            PRIVATE WSWRAP_NO_SSL)
endif()


# find zlib and enable compression support if found
find_package(ZLIB)
if(ZLIB_FOUND)
    target_link_libraries(ArchipelagoHitmanCompanion PRIVATE ZLIB::ZLIB)
else()
    target_compile_definitions(ArchipelagoHitmanCompanion
            PRIVATE WSWRAP_NO_COMPRESSION)
endif()


# Set UTF-8 flag.
set_target_properties(ArchipelagoHitmanCompanion PROPERTIES COMPILE_FLAGS "/utf-8")

# Add SDK and DX headers dependencies.
find_package(directx-headers CONFIG REQUIRED)

target_include_directories(ArchipelagoHitmanCompanion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/external/apclientpp/
    ${CMAKE_CURRENT_SOURCE_DIR}/external/valijson/include
    PRIVATE "${wswrap_SOURCE_DIR}/include"
    PRIVATE "${asio_SOURCE_DIR}/asio/include"
    PRIVATE ${websocketpp_SOURCE_DIR}
)

target_compile_definitions(ArchipelagoHitmanCompanion PRIVATE
    # Define any necessary preprocessor definitions here.
    ASIO_STANDALONE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WEBSOCKETPP_CPP11_STL_
)

target_link_libraries(ArchipelagoHitmanCompanion PRIVATE
    ZHMModSDK
    Microsoft::DirectX-Guids
    Microsoft::DirectX-Headers
    implot
    PRIVATE nlohmann_json::nlohmann_json
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    crypt32
)

install(TARGETS ArchipelagoHitmanCompanion
    RUNTIME DESTINATION bin
)

# Install the mod to the game folder when the `GAME_INSTALL_PATH` variable is set.
zhmmodsdk_install(ArchipelagoHitmanCompanion)
