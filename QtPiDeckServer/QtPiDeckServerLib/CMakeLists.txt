find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Network REQUIRED)

add_library(QtPiDeckServerLib STATIC
  src/ServerApplication.cpp
  src/Network/DeckServer.cpp
  src/Services/SettingsStorage.cpp
  src/ViewModels/SettingsViewModel.cpp
  include/ServerApplication.hpp
  include/Network/DeckServer.hpp
  include/Services/IServerSettingsStorage.hpp
  include/Services/SettingsStorage.hpp
  include/ViewModels/SettingsViewModel.hpp
  qml.qrc
  )

target_include_directories(QtPiDeckServerLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(QtPiDeckServerLib
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Network
    QtPiDeckCommon
    PRIVATE)

get_target_property(QtPiDeckCommon_SOURCE_DIR QtPiDeckCommon SOURCE_DIR)
set(QML_IMPORT_PATH ${QtPiDeckCommon_SOURCE_DIR}/qml/components CACHE STRING "Qt Creator qml import paths" FORCE)
