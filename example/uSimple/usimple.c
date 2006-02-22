/*
 * A microscopic ARToolkit example
 * 
 */

#include "artoolkit.h"
#include "../common/bmp.c"

int main(int argc, char** argv) 
{
	/* load frame */
	ARFrame frame;
	artMarker *hiro_marker, *kanji_marker;
	
	int i = 0;

	arToolkit *art = 0; 	

	/* initialise a new AR Toolkit state */
	art = artCreate();

	/* set debugging */
	artSetInteger(art,artDebug,0);

	/* load an image */
	bmpLoad("Data/hiro.bmp",&frame);

	/* load a pattern */
	hiro_marker = artLoadMarker(art,"Data/patt.hiro",80.0);

	/* load another marker */
	kanji_marker = artLoadMarker(art,"Data/patt.kanji",80.0);

	/* load a camera pattern */
	artLoadCameraParameter(art,"Data/camera_para.dat");
	artParamChangeSize(art,frame.width,frame.height);
	
	for (i = 0; i < 20; i++) 
	{
		/* set the current image (well, it doesn't change) */
		artSetImage(art,frame.frame,frame.width,frame.height,ART_PIXFORMAT_RGB);

		/* detect the marker */
		artDetect(art);

		/* get the marker info */
		artUpdateMarker(art,ART_BASIC);

		/* Hiro is the Hero! :) */
		if (hiro_marker->visible) printf("Hiro!\n");

	};
    
	/* close the handle for AR Toolkit */
	artShutdown(art);

	return 0;

};
