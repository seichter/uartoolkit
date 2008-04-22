set(LIB_NAME uartoolkit)

set(LEGACY_CODE 
	${CMAKE_CURRENT_SOURCE_DIR}/../AR/arDetectMarker2.c 
	${CMAKE_CURRENT_SOURCE_DIR}/../AR/arUtil.c
	)

file(GLOB HDRS *.h)
file(GLOB HDRS ${HDRS} ${CMAKE_SOURCE_DIR}/include/artoolkit/*.h)
file(GLOB HDRS ${HDRS} ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB SRCS *.c)

add_definitions(-DARTOOLKIT_BUILD_LIB=1)

add_library(${LIB_NAME} SHARED ${HDRS} ${SRCS} ${LEGACY_CODE})

set_target_properties(${LIB_NAME} PROPERTIES ENABLE_EXPORTS 1)



