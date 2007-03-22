
/* includes from SDL */
#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL/SDL.h>
#endif

/* include from IM */
#include <im.h>
#include <im_capture.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_format_avi.h>
#include <im_util.h>


#include <stdlib.h>

/* now uARToolkit */
#include <artoolkit.h>

/* Global Variables */
SDL_Surface *screen;
SDL_Surface *image;
SDL_Surface *cursor;

artMarker   *marker[2];
arToolkit	*art;

imVideoCapture	*video_capture = 0;    /* capture control */
imImage			*video_image = 0;         /* capture buffer  */


/* Returns a capture device */
int imGetCapture()
{
	int i;
	int cap_count = imVideoCaptureDeviceCount();

	printf("Found %d device(s)\n",cap_count);

	if (cap_count == 1)  /* only one device */
	return 0;

	printf("Enter the capture device number to use:\n");
	for (i = 0; i < cap_count; i++)
	{
		printf("  %s\n", imVideoCaptureDeviceDesc(i));
	}

	printf("  > ");
	scanf("%d", &i);
	if (i < 0 || i >= cap_count) {
		return 0;
	}

	return i;
}

/* initialise the video */
int imInit(int *width, int *height)
{
    /* creates an IM video capture manager */
    video_capture = imVideoCaptureCreate();
    if (!video_capture) {
		printf("No capture device found.\n"); 
		return 0;
    }

	/* conects the device */
    if (!imVideoCaptureConnect(video_capture, imGetCapture())) {
      imVideoCaptureDestroy(video_capture);
      printf("Can not connect to capture device.\n");  
	  return 0;
    }

	if (!imVideoCaptureLive(video_capture, 1)) {
		imVideoCaptureDisconnect(video_capture);
		imVideoCaptureDestroy(video_capture);
		printf("Can not activate capturing.\n");  
		return 0;
    }

	/* retrieve the image size */
	imVideoCaptureGetImageSize(video_capture, width, height);

	/* alocates the buffers */
	video_image = imImageCreate(*width,*height, IM_RGB | IM_PACKED, IM_BYTE);

	return 1;
}

void artInit() {

	int _width, _height;

	// new AR Toolkit handle
	art = artCreate();

	imInit(&_width,&_height);
	
	artSetInteger(art,artDebug,0);

	// load a pattern
	marker[0] = artLoadMarker(art,"Data/patt.hiro",80.0);
	// load a pattern
	marker[1] = artLoadMarker(art,"Data/patt.kanji",80.0);
	
	// change the size of the camera picture
	artLoadCameraParameter(art,"Data/camera_para.dat");
	artParamChangeSize(art,_width,_height);

	printf("%dx%d\n",_width,_height);

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


	// Attempt to create a window with 32bit pixels of same size of the image.
	screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE);
  
	// If we fail, return error.
	if ( screen == NULL ) 
	{
		fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
		exit(1);
	}
	
    	
	// load the hiro bitmap
	cursor = SDL_LoadBMP("Data/cursor.bmp");

	if (!cursor) {
	
		fprintf(stderr, "Unable to load file\n");

		exit(1);
	}

	SDL_SetColorKey(cursor, SDL_SRCCOLORKEY, SDL_MapRGB(cursor->format,255,255,255));

	
	Uint32 rmask, gmask, bmask, amask;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000;
	gmask = 0x00ff0000;
	bmask = 0x0000ff00;
	amask = 0x000000ff;

	printf("Use Big Endian\n");
#else
	rmask = 0x000000ff;
	gmask = 0x0000ff00;
	bmask = 0x00ff0000;
	amask = 0xff000000;

	printf("Not Big Endian\n");

#endif

	image = SDL_CreateRGBSurface(SDL_HWSURFACE, 
		video_image->width, video_image->height, 24, 
		rmask, gmask, bmask, 1);

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
	if (marker[0]->visible) 
	{
				
		// get the coordinates from the marker
		dest.x = (int)marker[0]->pos[0];
		dest.y = (int)marker[0]->pos[1];

		dest.w = cursor->w;
		dest.h = cursor->h;
		
		// blit the cursor onto the screen 
		SDL_BlitSurface(cursor, 0, screen, &dest);
		SDL_UpdateRects(screen, 1, &dest);	
	}

	// check if the marker is visible	
	if (marker[1]->visible) 
	{
				
		// get the coordinates from the marker
		dest.x = (int)marker[1]->pos[0];
		dest.y = (int)marker[1]->pos[1];

		dest.w = cursor->w;
		dest.h = cursor->h;
		
		// blit the cursor onto the screen 
		SDL_BlitSurface(cursor, 0, screen, &dest);
		SDL_UpdateRects(screen, 1, &dest);
	}
	
	// Unlock if needed
	if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);

	// Tell SDL to update the whole screen
	// SDL_UpdateRect(screen, 0, 0, 640, 480);    
}


// Entry point
int main(int argc, char *argv[])
{

	artInit();

	sdlInit();	

	// Main loop: loop forever.
	while (1)
	{

		if (imVideoCaptureLive(video_capture, -1))
		{
			/* capture a frame */
			imVideoCaptureFrame(video_capture, (unsigned char*)video_image->data[0], IM_RGB | IM_PACKED, 1000);

			image->pixels = video_image->data[0];
			
		}

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

