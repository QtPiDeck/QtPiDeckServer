find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)

# temporal solution while waiting for Qt6::WebSockets
if (${QT_VERSION_MAJOR} EQUAL 5)
    find_package(QT NAMES Qt5 COMPONENTS Quick Network WebSockets QuickControls2 REQUIRED)
    find_package(Qt5 COMPONENTS Quick Network WebSockets QuickControls2 REQUIRED)
else()
    find_package(QT NAMES Qt6 COMPONENTS Quick Network QuickControls2 REQUIRED)
    find_package(Qt6 COMPONENTS Quick Network QuickControls2 REQUIRED)
endif()

find_package(Boost REQUIRED)

add_library(QtPiDeckServerLib STATIC
    src/ServerApplication.cpp
    src/Network/DeckServer.cpp
    src/Network/Obs/ObsWebsocketClient.cpp
    src/Network/Obs/ObsObjectsParsing.cpp
    src/Services/SettingsStorage.cpp
    $<$<EQUAL:${QT_VERSION_MAJOR},5>:src/Services/WebSocketQt.cpp>
    $<$<EQUAL:${QT_VERSION_MAJOR},6>:src/Services/WebSocketpp.cpp>
    src/ViewModels/SettingsViewModel.cpp
    include/ServerApplication.hpp
    include/Bus/ObsMessages.hpp
    include/Network/DeckServer.hpp
    include/Network/Obs/GetAuthRequiredResponse.hpp
    include/Network/Obs/ObsObjectsParsing.hpp
    include/Network/Obs/ObsRequests.hpp
    include/Network/Obs/ObsResponseStatus.hpp
    include/Network/Obs/ObsWebsocketClient.hpp
    include/Services/IObsMessageIdGenerator.hpp
    include/Services/IServerSettingsStorage.hpp
    include/Services/IWebSocket.hpp
    $<$<EQUAL:${QT_VERSION_MAJOR},5>:include/Services/WebSocketQt.hpp>
    $<$<EQUAL:${QT_VERSION_MAJOR},6>:include/Services/WebSocketpp.hpp>
    include/Services/ObsMessageIdGenerator.hpp
    include/Services/SettingsStorage.hpp
    include/ViewModels/SettingsViewModel.hpp
    include/QtPiDeckServerDefinitions.hpp
    qml.qrc)

target_include_directories(QtPiDeckServerLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (${QT_VERSION_MAJOR} EQUAL 6)
    find_package(websocketpp CONFIG REQUIRED)
endif()

target_link_libraries(QtPiDeckServerLib
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::QuickControls2
    $<$<EQUAL:${QT_VERSION_MAJOR},5>:Qt5::WebSockets>
    $<$<EQUAL:${QT_VERSION_MAJOR},6>:websocketpp::websocketpp>
    Boost::headers
    QtPiDeckCommon
    QtDefinitions)

set_warnings_flags(QtPiDeckServerLib)

if(CODECOV AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(QtPiDeckServerLib PUBLIC --coverage)
    target_link_options(QtPiDeckServerLib PUBLIC --coverage)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_definitions(QtPiDeckServerLib
        PUBLIC
        APPLE_CLANG)
endif()

get_target_property(QtPiDeckCommon_SOURCE_DIR QtPiDeckCommon SOURCE_DIR)
set(QML_IMPORT_PATH ${QtPiDeckCommon_SOURCE_DIR}/qml/components CACHE STRING "Qt Creator qml import paths" FORCE)
