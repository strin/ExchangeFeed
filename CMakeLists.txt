CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT (Feed)

set(CMAKE_PREFIX_PATH /opt/local ${CMAKE_PREFIX_PATH}) 
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules) 

find_package(Boost REQUIRED)
message("Include dirs of boost: " ${Boost_INCLUDE_DIRS} )
message("Libs of boost: " ${Boost_LIBRARIES} )

include_directories(
  "inc"
  ${Boost_INCLUDE_DIRS}
)

message(STATUS ${LIBS})

IF(CMAKE_COMPILER_IS_GNUCXX)
  message("Linux: Found g++ version " ${CMAKE_CXX_COMPILER_VERSION})
  ADD_DEFINITIONS("-std=c++11" "-g" "-O3" "-fPIC" "-Wno-deprecated-register" "-Wno-overloaded-virtual" "-Wno-write-strings")
ELSE("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  message("Mac OS X: Found clang++ version" ${CMAKE_CXX_COMPILER_VERSION})
  ADD_DEFINITIONS("-std=c++11" "-g" "-O0" "-fPIC" "-Wno-deprecated-register" "-Wno-overloaded-virtual" "-Wno-write-strings")
ELSE()
  MESSAGE(FATAL_ERROR "CMakeLists.txt has not been tested/written for your compiler.")
ENDIF()

ADD_EXECUTABLE(feed
  main.cpp
)

target_link_libraries(feed
  ${LIBS}
  ${Boost_LIBRARIES}
)

