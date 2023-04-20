add_library(
  ALSAInterface
  OBJECT
  ALSA.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:ALSAInterface>
  PARENT_SCOPE
)

target_link_libraries(
  ALSAInterface
  PRIVATE
  ${ALSA_LIBRARIES}
)