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

target_link_libraries(Decoder ${AVUTIL_LIBRARIES})
target_link_libraries(Decoder ${AVCODEC_LIBRARIES})
target_link_libraries(Decoder ${AVFORMAT_LIBRARIES})