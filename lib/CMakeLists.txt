add_subdirectory(WAVDecoder)

add_library(nocturne STATIC ${ALL_OBJECT_FILES})

set(NOCTURNE_LIBS
  WAVDecoder
)

target_link_libraries(nocturne ${NOCTURNE_LIBS})

target_include_directories(
  nocturne PUBLIC 
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)