set(NAIVER_PLAYER_SOURCES main.cc)
add_executable(naiver_player ${NAIVER_PLAYER_SOURCES})

target_link_libraries(naiver_player nocturne)