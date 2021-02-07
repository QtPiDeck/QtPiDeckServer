set(TESTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TESTED_TARGET QtPiDeckServerLib)

macro(add_catch_cpp_test)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)

    get_test_name(TEST_NAME ${CMAKE_CURRENT_SOURCE_DIR})
    get_test_exe_name(EXE_NAME ${TEST_NAME})

    add_executable(${EXE_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
    add_test(NAME ${TEST_NAME} COMMAND ${EXE_NAME})

    if(MINGW)
        target_compile_options(${EXE_NAME} PRIVATE -Wa,-mbig-obj)
    endif()

    find_package(Catch2 CONFIG REQUIRED)

    target_link_libraries(${EXE_NAME}
        PRIVATE
        Catch2::Catch2
        ${TESTED_TARGET})
endmacro()

add_subdirectory(QtPiDeck/Network/Obs/ObsWebsocketClientTests)
add_subdirectory(QtPiDeck/Services/ObsMessageIdGeneratorTests)
