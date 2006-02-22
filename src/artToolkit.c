#include "artoolkit/ar.h"
#include "artoolkit/types.h"
#include "artoolkit/enums.h"
#include "artoolkit/external.h"

#include "artCode.h"
#include "artMatrix.h"

#include <string.h>


arToolkit* artCreate() {
	
	arToolkit* state = malloc(sizeof(arToolkit));

	/* 
	 * do not assert here, let the application developer 
	 * decide what to do in case of allocation error.
	 */
	 
	 
	/* 
	 * if we successfully allocated memory set basic things
	 * up in order to be up and running.
	 */
	if (state) {

		memset(state,0,sizeof(arToolkit));

		state->thresh = 100;
		state->debug = 0;

		state->wmarker_info = malloc(AR_SQUARE_MAX * sizeof(ARMarkerInfo));
		
		assert(state->wmarker_info);
		
		state->wmarker_info2 = malloc(AR_SQUARE_MAX * sizeof(ARMarkerInfo2));
		
		assert(state->wmarker_info2);
		
		state->prev_info = malloc(AR_SQUARE_MAX * sizeof(arPrevInfo));
		
		assert(state->prev_info);

		state->l_image = malloc((1024 * 1024) * sizeof(ARInt16));
		
		assert(state->l_image);
		

	};

    return state;
};

void artSetInteger(arToolkit* state,unsigned int name, int value)
{
	/*
	 * assert, that should hint a developer for a mistake 
	 */
	assert(state);

	switch (name) {
		case artThreshold :
			state->thresh = value;
			break;
		case artDebug : 
			state->debug = value;
			break;
		default:
			/* 
			 * here should be a logging handle within the state
			 * that captures a runtime error. 
			 */
			break;
	};
};

int artGetInteger(arToolkit* state,unsigned int name) 
{
	assert(state);

	switch (name) {
		case artThreshold :
			return state->thresh;
			break;
		case artDebug : 
			return state->debug;
			break;
		default:			
			break;
	};

	return artError;
};

int artLoadCameraParameter(arToolkit* state,const char* filename)
{
	assert(state);

	return arParamLoad(filename, 1, &state->wparam);
};

void artParamChangeSize(arToolkit* state, int x, int y)
{
	assert(state);

	arParamChangeSize(&state->wparam, x, y, &state->wparam);
    arInitCparam(&state->wparam);
};


artMarker *artLoadMarker(arToolkit* state,const char* filename, double size)
{
	artMarker *marker = 0;

	assert(state);

	marker = malloc(sizeof(artMarker));

	if (marker) 
	{
		memset(marker, 0, sizeof(artMarker));

		marker->visible = 0;
		marker->id = arLoadPattNew(state,filename);
		marker->width = size;

		if (0 == state->markercount) 
		{
			state->markers = malloc(sizeof(artMarker));

		} else {
			
			artMarker **temp = realloc(state->markers,sizeof(artMarker) * (state->markercount + 1));

			if (temp) 
			{
				state->markers = temp;				
				
			} else {

				if (state->debug) printf("::realloc Error");				
				return 0;
			}
		}

		state->markers[state->markercount] = marker;
		state->markercount++;

	}

	return marker;
}

void artShutdown(arToolkit *state) {

	if (state) 
	{
		/* Markers are not freed' because 
		   they could be shared!
		 */
		free(state->wmarker_info);
		free(state->wmarker_info2);
		free(state->l_image);
		
	};

	free(state);
};

void artSetImage(arToolkit* state, unsigned char* image, int width, int height, unsigned int format) 
{
	assert(state);

	state->image.frame = image;
	state->image.width = width;
	state->image.height = height;
	state->image.format = format;
	state->image.pix_size = 3;
}

void artUpdateMarker(arToolkit *state, int type) 
{
  
	register int k;
	register int j = 0;
	register unsigned int i = 0;

	artMarker **marker = state->markers;

	for (i = 0; i < state->markercount; i++) 
	{
		k = -1;

		for (j = 0; j < state->wmarker_num; j++)
		{			

			if (state->wmarker_info[j].id == marker[i]->id) 
			{		
				/* First marker detected. */
				if (k == -1) k = j;  
				else
					/* Higher confidence marker detected. */ 
					if (state->wmarker_info[j].cf > state->wmarker_info[k].cf) 
						k = j;
				}
		}
		
		marker[i]->visible = (k != -1) ? 1 : 0;

		if (marker[i]->visible && type) {
	
			marker[i]->pos[0] = state->wmarker_info[k].pos[0];
			marker[i]->pos[1] = state->wmarker_info[k].pos[1];
			
			switch (type) 
			{
				case ART_OPENGL:
					artGetOpenGLMatrix(&state->wparam,
						&(state->wmarker_info[k]),
						marker[i]->center, 
						marker[i]->width,
						marker[i]->transform);
					break;
				default:
					break;
			};
		}
	}
}

