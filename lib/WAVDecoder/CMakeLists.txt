add_library(
  WAVDecoder
  OBJECT
  Decoder.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:WAVDecoder>
  PARENT_SCOPE
)