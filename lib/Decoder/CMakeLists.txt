add_library(
  Decoder
  OBJECT
  WAVDecoder.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:Decoder>
  PARENT_SCOPE
)