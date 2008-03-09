#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

/* include from VX */
#include <vx.h>

/* include from ARToolkit */
#include <artoolkit.h>


/* a state variable */
arToolkit *art = 0;

int initialised = 0;

/* info about the image */
int width, height;

/* pointer to markers */
artMarker *hiro_marker, *kanji_marker;



int 
vxCaptureCallback(vxSource* src, vxFrame* frame, void *userdata)
{

	if (!initialised) {

		/* configure the size we are actually using */
		artParamChangeSize(art,frame->info.width,frame->info.height);
		initialised++;

	}

	artSetImage(art, frame->image.data, 
		frame->info.width, 
		frame->info.height, ART_PIXFORMAT_RGB);

	/* detect the marker */
	artDetect(art);

	/* generate info for the markers */
	artUpdateMarker(art,ART_BASIC);

	/* now the time has come to see what happened */
	if (hiro_marker->visible)
	{
		printf("Hiro!\n");
	};

	return vxNext;
}



int main(int argc, char* argv[])
{

	vxModule* module;
	vxSource* source;


	/* initialise a new AR Toolkit state */
	art = artCreate();

	/* set debugging */
	artSetInteger(art, artDebug, 0);

	/* load a pattern onto a marker */
	hiro_marker = artLoadMarker(art,"Data/patt.hiro",80.0);


	/* load the camera parameter */
	artLoadCameraParameter(art,"Data/camera_para.dat");


	/** register a module */
	module = vxLoadModule(argv[1]);

	/** create a source from that module */
	source = vxCreate(module);

	/** configure the source */
	vxConfig(source, "v4lsrc ! ffmpegcolorspace ! capsfilter caps=video/x-raw-rgb,bpp=24 ! identity name=vxsink ! ffmpegcolorspace ! autovideosink");

	/** start capturing */
	vxStart(source,&vxCaptureCallback,0);
	

	while (vxOK == vxUpdate(source,vxNext)) {

	};


}
