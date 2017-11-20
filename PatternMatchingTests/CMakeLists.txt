# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/PndTrackers/PatternMatchingTests
  ${CMAKE_SOURCE_DIR}/stt
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/SttData
  ${CMAKE_SOURCE_DIR}/pnddata/FtsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  #${CMAKE_SOURCE_DIR}/recotasks
  #${CMAKE_SOURCE_DIR}/trackbase
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libSds ######################################
set(SRCS

Pattern.cxx
PatternCounter.cxx
PatternDBGenerator.cxx
PatternDBMerger.cxx
PatternMatcher.cxx
TestClass.cxx

)

set(LINKDEF  PatternTestsLinkDef.h)
set(LIBRARY_NAME PatternTests)

set(DEPENDENCIES Base)
#set(DEPENDENCIES Base GeoBase ParBase PndData generalTools)

GENERATE_LIBRARY()
 
