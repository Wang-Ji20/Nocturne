set(NAIVE_PLAYER_SOURCES main.cc)
add_executable(naive_player ${NAIVE_PLAYER_SOURCES})

target_link_libraries(naive_player nocturne)