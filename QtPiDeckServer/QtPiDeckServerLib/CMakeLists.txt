find_package(QT NAMES Qt5 Qt6 COMPONENTS Quick Network WebSockets QuickControls2 REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Network WebSockets QuickControls2 REQUIRED)

find_package(Boost REQUIRED)

add_library(QtPiDeckServerLib STATIC
    src/ServerApplication.cpp
    src/Network/DeckServer.cpp
    src/Network/Obs/ObsWebsocketClient.cpp
    src/Network/Obs/ObsObjectsParsing.cpp
    src/Services/SettingsStorage.cpp
    src/Services/WebSocketQt.cpp
    src/ViewModels/SettingsViewModel.cpp
    include/ServerApplication.hpp
    include/Bus/ObsMessages.hpp
    include/Network/DeckServer.hpp
    include/Network/Obs/Models/GetAuthRequiredResponse.hpp
    include/Network/Obs/ObsObjectsParsing.hpp
    include/Network/Obs/ObsRequests.hpp
    include/Network/Obs/Models/ObsResponseStatus.hpp
    include/Network/Obs/ObsWebsocketClient.hpp
    include/Services/IObsMessageIdGenerator.hpp
    include/Services/IServerSettingsStorage.hpp
    include/Services/IWebSocket.hpp
    include/Services/WebSocketQt.hpp
    include/Services/ObsMessageIdGenerator.hpp
    include/Services/SettingsStorage.hpp
    include/ViewModels/SettingsViewModel.hpp
    include/QtPiDeckServerDefinitions.hpp
    qml.qrc)

target_include_directories(QtPiDeckServerLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(QtPiDeckServerLib
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::QuickControls2
    Qt${QT_VERSION_MAJOR}::WebSockets
    Boost::headers
    QtPiDeckCommon
    QtDefinitions)

set_warnings_flags(QtPiDeckServerLib)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" 
   AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL "13" 
   AND Boost_VERSION_STRING VERSION_EQUAL "1.76") # change boost comparison if needed
   message(STATUS "BOOST_ASIO_HAS_STD_INVOKE_RESULT is set to 1")
   target_compile_definitions(QtPiDeckServerLib PUBLIC BOOST_ASIO_HAS_STD_INVOKE_RESULT=1)
endif()

if(CODECOV AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(QtPiDeckServerLib PUBLIC --coverage)
    target_link_options(QtPiDeckServerLib PUBLIC --coverage)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    target_compile_definitions(QtPiDeckServerLib
        PUBLIC
        APPLE_CLANG)
endif()

if(NOT CXX20RANGES)
    target_include_directories(QtPiDeckServerLib PUBLIC ${NANORANGE_DIR})
    target_compile_definitions(QtPiDeckServerLib PUBLIC NANORANGE)
endif()

get_target_property(QtPiDeckCommon_SOURCE_DIR QtPiDeckCommon SOURCE_DIR)
set(QML_IMPORT_PATH ${QtPiDeckCommon_SOURCE_DIR}/qml/components CACHE STRING "Qt Creator qml import paths" FORCE)
