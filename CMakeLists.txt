cmake_minimum_required(VERSION 3.15)

project(ArchipelagoHitmanCompanion CXX)

# Find latest version at https://github.com/OrfeasZ/ZHMModSDK/releases
# Set ZHMMODSDK_DIR variable to a local directory to use a local copy of the ZHMModSDK.
set(ZHMMODSDK_VER "v4.0.0-rc.3")
include(cmake/setup-zhmmodsdk.cmake)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/external/json/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/valijson/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/websocketpp
    ${CMAKE_CURRENT_SOURCE_DIR}/external/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/wswrap/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/zlib/include
)

target_compile_definitions(ArchipelagoHitmanCompanion PRIVATE
    # Define any necessary preprocessor definitions here.
    ASIO_STANDALONE
    _WEBSOCKETPP_CPP11_STL_
    WSWRAP_NO_SSL
    WSWRAP_NO_COMPRESSION
    AP_PREFER_UNENCRYPTED
    APCLIENT_DEBUG
)

target_link_libraries(ArchipelagoHitmanCompanion
    ZHMModSDK
    Microsoft::DirectX-Guids
    Microsoft::DirectX-Headers
    implot
)

install(TARGETS ArchipelagoHitmanCompanion
    RUNTIME DESTINATION bin
)

# Install the mod to the game folder when the `GAME_INSTALL_PATH` variable is set.
zhmmodsdk_install(ArchipelagoHitmanCompanion)
