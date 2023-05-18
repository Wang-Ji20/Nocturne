add_library(
  Decoder
  OBJECT
  WAVDecoder.cc
  FLACDecoder.cc
  RawStream.cc
  FFDecoder.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:Decoder>
  PARENT_SCOPE
)

target_link_libraries(Decoder ${FFMPEG_LIBRARIES})