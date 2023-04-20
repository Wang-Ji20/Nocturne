FLEX_TARGET(MyScanner lexer.ll  ${CMAKE_CURRENT_BINARY_DIR}/lexer.cc)

set(NAIVE_PLAYER_SOURCES main.cc lexer.cc)
add_executable(naive_player ${NAIVE_PLAYER_SOURCES})

target_link_libraries(naive_player nocturne)