find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

add_executable(QtPiDeckServerCodeTests
    src/main.cpp
    src/Services/ObsMessageIdGeneratorTests.cpp
    src/Network/Obs/ObsWebsocketClientTests.cpp
    src/Network/Obs/Models/GetAuthRequiredResponseTests.cpp
    src/Network/Obs/ObsObjectsParsingTests.cpp
    src/ViewModels/SettingsViewModelTests.cpp)

target_link_libraries(QtPiDeckServerCodeTests
    PRIVATE
      #Qt${QT_VERSION_MAJOR}::Quick
      BoostTestWrapper
      QtPiDeckServerLib
      CodeTestsUtils)

set_warnings_flags(QtPiDeckServerCodeTests)
add_test(NAME QtPiDeckServerCodeTests COMMAND QtPiDeckServerCodeTests)
target_compile_definitions(QtPiDeckServerCodeTests
    PRIVATE
      $<$<BOOL:${WIN32}>:BOOST_TEST_DYN_LINK>
      $<$<AND:$<BOOL:${WIN32}>,$<EQUAL:${QT_VERSION_MAJOR},5>>:NOMINMAX>)

# visual studio test manager requires to have these libs in PATH
# so we just copy required libs to test binary
if(VISUAL_STUDIO)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        set(RELEASE_FLAG "--release")
    endif()

    add_custom_target(
        CopyQtPiDeckServerTests
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:QtPiDeckCommon>
            #$<TARGET_FILE:Boost::log>
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS QtPiDeckCommon)

    add_dependencies(QtPiDeckServerCodeTests CopyQtPiDeckServerTests)

    add_custom_command(
        TARGET QtPiDeckServerCodeTests POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            --verbose 0
            --qmldir ${CMAKE_CURRENT_SOURCE_DIR}
            --no-compiler-runtime
            ${RELEASE_FLAG}
            \"$<TARGET_FILE:QtPiDeckServerCodeTests>\"
            \"$<TARGET_FILE:QtPiDeckCommon>\")

    unset(RELEASE_FLAG)
endif()
