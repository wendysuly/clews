SET(CMAKE_CXX_FLAGS "-Wall -std=c++0x")
string(TOLOWER ${PROJECT_NAME} TARGET_NAME)

# Grab all of the CPP files in src/Common and subdirectories; store in CPP_SRCS
FILE(GLOB_RECURSE COMMON_CPP_SRCS src/Common/*.cpp)
FILE(GLOB_RECURSE COMMON_C_SRCS src/Common/*.c)

FILE(GLOB_RECURSE COMMON_CPP_HDRS src/Common/*.hpp)
FILE(GLOB_RECURSE COMMON_C_HDRS src/Common/*.h)

SET(COMMON_SRCS ${COMMON_CPP_SRCS} ${COMMON_C_SRCS})
SET(COMMON_HDRS ${COMMON_CPP_HDRS} ${COMMON_C_HDRS})

include_directories(${ADDITIONAL_INCLUDE_DIRS})

IF(WIN32)
SET(PLATFORM "WIN")
ELSE()
SET(PLATFORM "NIX")
ENDIF()

MESSAGE("Platform is: ${PLATFORM}")
# Grab all of the CPP files in src/${PLATFORM} and subdirectories; store in CPP_SRCS
FILE(GLOB_RECURSE PLATFORM_CPP_SRCS src/${PLATFORM}/*.cpp)
FILE(GLOB_RECURSE PLATFORM_C_SRCS src/${PLATFORM}/*.c)

FILE(GLOB_RECURSE PLATFORM_CPP_HDRS src/${PLATFORM}/*.hpp)
FILE(GLOB_RECURSE PLATFORM_C_HDRS src/${PLATFORM}/*.h)

SET(PLATFORM_SRCS ${PLATFORM_CPP_SRCS} ${PLATFORM_C_SRCS})
SET(PLATFORM_HDRS ${PLATFORM_CPP_HDRS} ${PLATFORM_C_HDRS})

MESSAGE(STATUS "CXX Flags: " ${CMAKE_CXX_FLAGS})


function(clews_add_qt_resources)
    FILE(GLOB_RECURSE COMMON_RESOURCE_FILES src/Common/*.qrc)
    FILE(GLOB_RECURSE PLATFORM_RESOURCE_FILES src/${PLATFORM}/*.qrc)

    QT5_ADD_RESOURCES(COMMON_RESOURCES ${COMMON_RESOURCE_FILES})
    QT5_ADD_RESOURCES(PLATFORM_RESOURCES ${PLATFORM_RESOURCE_FILES})

    SET(QT_RESOURCES ${COMMON_RESOURCES} ${PLATFORM_RESOURCES} PARENT_SCOPE)
    MESSAGE(STATUS "Found Qt Resources: ${QT_RESOURCES}")
endfunction(clews_add_qt_resources)

function(clews_add_lib TARGETSUFFIX)
    ADD_LIBRARY(${TARGET_NAME} SHARED ${COMMON_SRCS} ${COMMON_HDRS} ${PLATFORM_SRCS} ${PLATFORM_HDRS})

    IF(WIN32)
    MESSAGE(STATUS "Install targets not supported on Windows")
    ELSE()
    INSTALL(FILES ${COMMON_HDRS} ${PLATFORM_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${TARGETSUFFIX}/${TARGET_NAME})

    INSTALL(TARGETS ${TARGET_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGETSUFFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGETSUFFIX}
        COMPONENT library
    )
    ENDIF()
endfunction(clews_add_lib)

function(clews_add_static_lib TARGETSUFFIX)
    ADD_LIBRARY(${TARGET_NAME} STATIC ${COMMON_SRCS} ${COMMON_HDRS} ${PLATFORM_SRCS} ${PLATFORM_HDRS})

    IF(WIN32)
    MESSAGE(STATUS "Install targets not supported on Windows")
    ELSE()
    INSTALL(FILES ${COMMON_HDRS} ${PLATFORM_HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/${TARGETSUFFIX}/${TARGET_NAME})

    INSTALL(TARGETS ${TARGET_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGETSUFFIX}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${TARGETSUFFIX}
        COMPONENT library
    )
    ENDIF()
endfunction(clews_add_static_lib)

function(clews_add_executable)
  MESSAGE(STATUS "ADD_EXECUTABLE(${TARGET_NAME} ${COMMON_SRCS} ${COMMON_HDRS} ${PLATFORM_SRCS} ${PLATFORM_HDRS}  ${QT_RESOURCES})")
  ADD_EXECUTABLE(${TARGET_NAME} ${COMMON_SRCS} ${COMMON_HDRS} ${PLATFORM_SRCS} ${PLATFORM_HDRS} ${QT_RESOURCES})
endfunction(clews_add_executable)

function(clews_target_link_libraries LINKTARGETS)
    TARGET_LINK_LIBRARIES(${TARGET_NAME} ${LINKTARGETS})
endfunction(clews_target_link_libraries)
