find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

function(add_server_cpp_test testName filename)
    add_executable(${testName}
                   ${filename})
    target_link_libraries(${testName}
        PRIVATE
        BoostTestWrapper
        QtPiDeckServerLib)
    set_warnings_flags(${testName})
    add_test(NAME ${testName} COMMAND ${testName})
    target_compile_definitions(${testName} PRIVATE BOOST_TEST_NO_MAIN BOOST_TEST_ALTERNATIVE_INIT_API)
endfunction()

add_server_cpp_test(ObsMessageIdGeneratorTests src/Services/ObsMessageIdGeneratorTests.cpp)
add_server_cpp_test(ObsWebsocketClientTests src/Network/Obs/ObsWebsocketClientTests.cpp)
add_server_cpp_test(GetAuthRequiredResponseTests src/Network/Obs/GetAuthRequiredResponseTests.cpp)

# visual studio test manager requires to have these libs in PATH
# so we just copy required libs to test binary
if(VISUAL_STUDIO)    
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Qml Gui Network REQUIRED)
    add_custom_command(
        TARGET ObsMessageIdGeneratorTests POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:QtPiDeckCommon>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Core>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Qml>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Gui>
        $<TARGET_FILE:Qt${QT_VERSION_MAJOR}::Network>
        $<TARGET_FILE_DIR:ObsMessageIdGeneratorTests>)
endif()
