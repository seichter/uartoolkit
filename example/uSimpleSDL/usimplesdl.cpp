#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

#include <stdlib.h>
#include <artoolkit.h>

SDL_Surface *screen;
SDL_Surface *image;
SDL_Surface *cursor;

artMarker   *marker;
arToolkit	*art;

void artInit() {

	// new AR Toolkit handle
	art = artCreate();
	
	artSetInteger(art,artDebug,0);

	// load a pattern
	marker = artLoadMarker(art,"Data/patt.hiro",80.0);
	
	// change the size of the camera picture
	artLoadCameraParameter(art,"Data/camera_para.dat");
	artParamChangeSize(art,image->w,image->h);

}

void sdlInit() {

	// Initialize SDL's subsystems - in this case, only video.
	if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
	{
		fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
		exit(1);
	}
	
	// Register SDL_Quit to be called at exit; makes sure things are
	// cleaned up when we quit.
	atexit(SDL_Quit);
    	
	// load the hiro bitmap
	image = SDL_LoadBMP("Data/hiro.bmp");	
	// load the hiro bitmap
	cursor = SDL_LoadBMP("Data/cursor.bmp");

	if (!image || !cursor) {
	
		fprintf(stderr, "Unable to load file\n");
	}
	
	// Attempt to create a window with 32bit pixels of same size of the image.
	screen = SDL_SetVideoMode(image->w, image->h, 32, SDL_HWSURFACE);
  
	// If we fail, return error.
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}

	
	SDL_SetColorKey(cursor, SDL_SRCCOLORKEY, SDL_MapRGB(cursor->format,0,0,255));

}

void sdlRender() {   

	// Display untainted
	SDL_Rect dest;

	// Lock surface if needed
	if (SDL_MUSTLOCK(screen))
		if (SDL_LockSurface(screen) < 0) return;


	// update the background image
	dest.x = 0;
	dest.y = 0;
	dest.w = image->w;
	dest.h = image->h;

	// blit the image onto the screen
	SDL_BlitSurface(image, 0, screen, &dest);
	SDL_UpdateRects(screen, 1, &dest);

	// check if the marker is visible	
	if (marker->visible) 
	{
				
		// get the coordinates from the marker
		dest.x = (int)marker->pos[0];
		dest.y = (int)marker->pos[1];

		dest.w = cursor->w;
		dest.h = cursor->h;
		
		// blit the cursor onto the screen 
		SDL_BlitSurface(cursor, 0, screen, &dest);
		SDL_UpdateRects(screen, 1, &dest);	
	}
	
	// Unlock if needed
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	// Tell SDL to update the whole screen
	SDL_UpdateRect(screen, 0, 0, 640, 480);    
}


// Entry point
int main(int argc, char *argv[])
{

	sdlInit();	
	artInit();

	// Main loop: loop forever.
	while (1)
	{
		// update the image
		artSetImage(art,(unsigned char*)image->pixels,image->w,image->h,ART_PIXFORMAT_RGB);		
		
		// detect the marker
		artDetect(art);
		
		// Update the marker info
		artUpdateMarker(art,ART_BASIC);

		// Render stuff
		sdlRender();

		// Poll for events, and handle the ones we care about.
		SDL_Event event;
		
		while (SDL_PollEvent(&event)) 
		{
			switch (event.type) 
			{
				case SDL_KEYDOWN:
					break;
			case SDL_KEYUP:
				// If escape is pressed, return (and thus, quit)
				if (event.key.keysym.sym == SDLK_ESCAPE)
				return 0;
				break;
			case SDL_QUIT:
				return(0);
			}
		}
	}
	return 0;
}

