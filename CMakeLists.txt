cmake_minimum_required(VERSION 3.16...3.27)
project(XRFPhosphate CXX)

option(WITH_GEANT4_UIVIS "Build with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 11.0 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 11.0 REQUIRED)
endif()
include(${Geant4_USE_FILE})

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(xrf xrf.cc ${sources} ${headers})
target_include_directories(xrf PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(xrf ${Geant4_LIBRARIES})

file(COPY ${PROJECT_SOURCE_DIR}/macros DESTINATION ${PROJECT_BINARY_DIR})
file(COPY ${PROJECT_SOURCE_DIR}/analysis DESTINATION ${PROJECT_BINARY_DIR})
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/out)
