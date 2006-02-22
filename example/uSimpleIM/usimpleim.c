#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/* include from IM */
#include <im.h>
#include <im_capture.h>
#include <im_image.h>
#include <im_convert.h>
#include <im_process.h>
#include <im_format_avi.h>
#include <im_util.h>

/* include from ARToolkit */
#include <artoolkit.h>

/* Global Variables */
imVideoCapture	*myVideoCap = 0;    /* capture control */
imImage			*image = 0;         /* capture buffer  */


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
    myVideoCap = imVideoCaptureCreate();
    if (!myVideoCap) {
		printf("No capture device found.\n"); 
		return 0;
    }

	/* conects the device */
    if (!imVideoCaptureConnect(myVideoCap, imGetCapture())) {
      imVideoCaptureDestroy(myVideoCap);
      printf("Can not connect to capture device.\n");  
	  return 0;
    }

	if (!imVideoCaptureLive(myVideoCap, 1)) {
		imVideoCaptureDisconnect(myVideoCap);
		imVideoCaptureDestroy(myVideoCap);
		printf("Can not activate capturing.\n");  
		return 0;
    }

	/* retrieve the image size */
	imVideoCaptureGetImageSize(myVideoCap, width, height);

	/* alocates the buffers */
	image = imImageCreate(*width,*height, IM_RGB, IM_BYTE);

	return 1;
}


int main(int argc, char* argv[])
{
	/* a state variable */
	arToolkit *art = 0;
	
	/* info about the image */
	int width, height;

	/* pointer to markers */
	artMarker *hiro_marker, *kanji_marker;

	/* initialise a new AR Toolkit state */
	art = artCreate();

	/* set debugging */
	artSetInteger(art, artDebug, 0);

	/* load a pattern onto a marker */
	hiro_marker = artLoadMarker(art,"Data/patt.hiro",80.0);

	/* load another */
	kanji_marker = artLoadMarker(art,"Data/patt.kanji",80.0);

	/* initialise IM library */
	if (!imInit(&width,&height)) 
	{
		/* gracefully shutdown AR Toolkit */
		artShutdown(art);

		/* terminate program */
		exit(1);
	};

	/* load the camera parameter */
	artLoadCameraParameter(art,"Data/camera_para.dat");

	/* configure the size we are actually using */
	artParamChangeSize(art,width,height);

	while (1) {

		if (imVideoCaptureLive(myVideoCap, -1))
		{
			/* capture a frame */
			imVideoCaptureFrame(myVideoCap, image->data[0], IM_RGB, 1000);

			/* set the current image (well, it doesn't change) */
			artSetImage(art, image->data[0], width, height, ART_PIXFORMAT_RGB);

			/* detect the marker */
			artDetect(art);

			/* generate info for the markers */
			artUpdateMarker(art,ART_BASIC);

			/* now the time has come to see what happened */
			if (hiro_marker->visible)
			{
				printf("Hiro!\n");
			};

			if (kanji_marker->visible)
			{
				printf("Kanji!\n");
			};

		};
	};


}