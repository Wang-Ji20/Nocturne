add_library(
  Effector
  OBJECT
  Debussy.cc
  interleaveEffector.cc
  sequenceEffector.cc
)

set(
  ALL_OBJECT_FILES
  ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:Effector>
  PARENT_SCOPE
)