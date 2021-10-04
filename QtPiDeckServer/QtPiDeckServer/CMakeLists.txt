find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick REQUIRED)

add_executable(QtPiDeckServer
    src/main.cpp)

target_link_libraries(QtPiDeckServer
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    QtPiDeckServerLib)

target_include_directories(QtPiDeckServer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_warnings_flags(QtPiDeckServer)


if(VISUAL_STUDIO)
    get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

    find_package(Boost REQUIRED COMPONENTS log)

    if (CMAKE_BUILD_TYPE EQUAL "RELEASE")
        set(RELEASE_FLAG "--release")
    endif()

    add_custom_command(
        TARGET QtPiDeckServer POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E
        env PATH="${_qt_bin_dir}" "${WINDEPLOYQT_EXECUTABLE}"
            --verbose 0
            --qmldir ${CMAKE_CURRENT_SOURCE_DIR}/../
            --no-compiler-runtime
            ${RELEASE_FLAG}
            \"$<TARGET_FILE:QtPiDeckServer>\"
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_FILE:QtPiDeckCommon>
            $<TARGET_FILE:Boost::log>
            ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Deploying Qt...")

    unset(RELEASE_FLAG)
endif()