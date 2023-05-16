add_subdirectory(Decoder)
add_subdirectory(ALSAInterface)

add_library(nocturne STATIC ${ALL_OBJECT_FILES})

set(NOCTURNE_LIBS
  Decoder
  ALSAInterface
)

target_link_libraries(nocturne ${NOCTURNE_LIBS})

target_include_directories(
  nocturne PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)