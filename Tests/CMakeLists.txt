set(TESTS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TESTED_TARGET QtPiDeckServerLib)

add_subdirectory(QtPiDeck/Network/Obs/ObsWebsocketClientTests)
add_subdirectory(QtPiDeck/Services/ObsMessageIdGeneratorTests)
