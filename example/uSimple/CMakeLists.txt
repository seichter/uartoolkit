ADD_EXECUTABLE(uart_simple usimple.c)

SET(ADD_LIBS)

IF(UNIX)
SET(ADD_LIBS m)
ENDIF(UNIX)

TARGET_LINK_LIBRARIES(uart_simple uartoolkit ${ADD_LIBS})

