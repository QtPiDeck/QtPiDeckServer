find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(QtPiDeckServerCodeTests 
               src/main.cpp
               src/Network/Obs/ObsWebsocketClientTests.cpp 
               src/Services/ObsMessageIdGeneratorTests.cpp)
add_test(NAME QtPiDeckServerCodeTests COMMAND QtPiDeckServerCodeTests)
target_link_libraries(QtPiDeckServerCodeTests
    PRIVATE
    BoostTestWrapper
    QtPiDeckServerLib)

target_compile_definitions(
    QtPiDeckServerCodeTests 
    PRIVATE
    BOOST_TEST_DYN_LINK)

# visual studio test manager requires to have these libs in PATH
# so we just copy required libs to test binary
if(VISUAL_STUDIO)    
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Qml Gui Network REQUIRED)
    add_custom_command(
        TARGET QtPiDeckServerCodeTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Qml>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Gui>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Network>
        $<TARGET_FILE_DIR:QtPiDeckServerCodeTests>)
endif()
