########################################################################
IF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
MESSAGE(FATAL_ERROR "Prevented in-tree built. This is bad practice.")
ENDIF(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})

########################################################################
# Project setup
########################################################################
CMAKE_MINIMUM_REQUIRED(VERSION 3.1)
PROJECT(DustHook CXX)
ENABLE_TESTING()

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${PROJECT_SOURCE_DIR})

add_subdirectory(FromDustHook)

# find_library(MPSSE MPSSE ${CMAKE_CURRENT_SOURCE_DIR}/../third_party/libMPSSE)
# if(${MPSSE} STREQUAL "MPSSE-NOTFOUND")
# message(FATAL_ERROR "Couldn't find the 'MPSSE' library")
# endif()

# target_link_libraries(RTIMULib ${MPSSE})
# SET_PROPERTY(TARGET RTIMULib PROPERTY LINKER_LANGUAGE CXX)
