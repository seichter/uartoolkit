add_executable(uart_simple usimple.c)

set(ADD_LIBS)

if(UNIX)
	SET(ADD_LIBS m)
endif(UNIX)

target_link_libraries(uart_simple uartoolkit ${ADD_LIBS})

