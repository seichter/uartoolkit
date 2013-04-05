find_package(SDL)

if(SDL_FOUND)

	include_directories(${SDL_INCLUDE_DIR})
	add_executable(uart_simple_sdl usimplesdl.cpp)

	set(ADD_LIBS)

	if(UNIX)
		SET(ADD_LIBS m)
	endif(UNIX)

	target_link_libraries(uart_simple_sdl uartoolkit ${ADD_LIBS} ${SDL_LIBRARY})
endif()
