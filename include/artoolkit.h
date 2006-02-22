#ifndef artoolkit_h
#define artoolkit_h

#ifdef __cplusplus
extern "C" {
#endif

/* include public interface */
#include "artoolkit/external.h"
#include "artoolkit/sysconfig.h"
#include "artoolkit/enums.h"
	
/**
 * handle for AR Toolkit state
 */
typedef struct arToolkit arToolkit;

/**
 * Initializes the internal structures of ARToolkit
 * \return 0 if unsuccessfull, otherwise a pointer to internal state
 */
ARTOOLKIT_API arToolkit *artCreate(); 


/**
 * Detect a marker
 * \param state current AR Toolkit state
 * \return -1 if an error occured or the number of markers detected
 */
ARTOOLKIT_API int artDetect(arToolkit *AR);


/**
 * Closes the handle for ARToolkit
 * \param AR AR Toolkit handle
 */
ARTOOLKIT_API void artShutdown(arToolkit *AR);


/**
 * Set internal value.
 * \param AR AR Toolkit handle
 * \param name name of the parameter to set
 * \param value value of the parameter
 */
ARTOOLKIT_API void artSetInteger(arToolkit *AR,unsigned int name,int value);

/**
 * Get internal value.
 * \param AR AR Toolkit handle
 * \param name name of the parameter
 * \return value of the parameter
 */
ARTOOLKIT_API int artGetInteger(arToolkit *AR,unsigned int name);

/**
 * Load a pattern onto a marker
 * \param AR AR Toolkit handle
 * \param filename filename for the pattern file
 */
ARTOOLKIT_API artMarker *artLoadMarker(arToolkit *AR, 
								const char *filename,
								double size);

/** 
 * Load the camera parameter for the camera
 * \param AR handle for AR Toolkit
 * \param filename name for the file
 */
ARTOOLKIT_API int artLoadCameraParameter(arToolkit *AR,const char *filename);

/**
 * Change the camera parameter
 * \param AR AR Toolkit handle
 * \param width width of the current camera picture
 * \param height height of the current camera picture
 */
ARTOOLKIT_API void artParamChangeSize(arToolkit *AR, int x, int y);

/**
 * Every time a new frame is provided by the capturing device 
 * this function should be called in order to sustain the internal
 * integrity regarding the format and address of the image.
 * \param AR the current AR Toolkit handle
 * \param image a pointer to the image 
 * \param width width of the image provided
 * \param height height of the image provided
 * \param format the actual format of the image
 */
ARTOOLKIT_API void artSetImage(arToolkit *AR, 
						unsigned char* image, 
						int width, int height, 
						unsigned int format);
/**
 * This function updates the internal marker information
 * and calculates a matrix for the the system indicated
 * by the second parameter.
 * \param state all markers belonging to this state will be updated
 * \param type type of transformation matrix calculated
 */
ARTOOLKIT_API void artUpdateMarker(arToolkit *AR, int type);


/**
 * Get a timer 
 * \return a time stamp
 */
ARTOOLKIT_API double artTimer();

/**

\mainpage uARToolkit API Documentation

\section intro Introduction
uARToolkit is a lightweight derivate of the original ARToolkit
by Hirokazu Kato and Mark Billinghurst. It reduces dependencies
on specific APIs to a very minimum. Furthermore, this version
is able to change video formats at runtime. uARToolkit is
distributed under the terms of the GPL.

\section features Features
	
\li pure ANSI C API
\li video format agnostic
\li graphics library agnostic
\li not depending on video capturing facilities 
\li not depending on graphics library

\section goal Goals

This derivate version was born out of necessity to 
provide a high performance and lightweight alternative
to the original AR Toolkit. Most developers do not use stereo, 
drawing code and video capture facilities within the 
original AR Toolkit.

\li remove all static variables of the original AR Toolkit
\li make all parameters and vision code configurable at runtime

\section drawback Drawbacks

If your are using this version of AR Toolkit you should be aware
of the drawbacks to fully understand what uARToolkit targets:

\li there is no video capture facility - this is out of scope for uARToolkit
\li there is no drawing code, however this version can generate matrices for OpenGL and DirectX
\li you are dealing with a 5% performance hit due to the configurability of uARToolkit on a frame to frame basis
\li no explicit stereo mode is provided. You need to use two arToolkit handles instead



\section authors Authors

\li Hirokazu Kato
\li Mark Billinghurst
\li Thomas Pintaric
\li Hartmut Seichter

\section example Examples

\example uSimple/usimple.c

An example


*/


#ifdef __cplusplus
}
#endif
#endif
