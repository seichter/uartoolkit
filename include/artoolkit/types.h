#ifndef types_h
#define types_h

#include "config.h"
#include "external.h"

/* overhead ARToolkit type*/
typedef char              ARInt8;
typedef short             ARInt16;
typedef int               ARInt32;
typedef unsigned char     ARUint8;
typedef unsigned short    ARUint16;
typedef unsigned int      ARUint32;


/** 
* \struct ARMarkerInfo
* \brief main structure for detected marker.
*
* Store information after contour detection (in idea screen coordinate, after distorsion compensated).
* \remark lines are represented by 3 values a,b,c for ax+by+c=0
* \param area number of pixels in the labeled region
* \param id marker identitied number
* \param dir Direction that tells about the rotation about the marker (possible values are 0, 1, 2 or 3). This parameter makes it possible to tell about the line order of the detected marker (so which line is the first one) and so find the first vertex. This is important to compute the transformation matrix in arGetTransMat().
* \param cf confidence value (probability to be a marker)
* \param pos center of marker (in ideal screen coordinates)
* \param line line equations for four side of the marker (in ideal screen coordinates)
* \param vertex edge points of the marker (in ideal screen coordinates)
*/
typedef struct {	
	int     area;
    int     id;
    int     dir;
    double  cf;
    double  pos[2];
    double  line[4][3];
    double  vertex[4][2];
} ARMarkerInfo;

/** \struct ARMarkerInfo2
* \brief internal structure use for marker detection.
* 
* Store information after contour detection (in observed screen coordinate, before distorsion correction).
* \param area number of pixels in the labeled region
* \param pos position of the center of the marker (in observed screen coordinates)
* \param coord_num numer of pixels in the contour.
* \param x_coord x coordinate of the pixels of contours (size limited by AR_CHAIN_MAX).
* \param y_coord y coordinate of the pixels of contours (size limited by AR_CHAIN_MAX).
* \param vertex position of the vertices of the marker. (in observed screen coordinates)
		 rem:the first vertex is stored again as the 5th entry in the array – for convenience of drawing a line-strip easier.
* 
*/
typedef struct {
    int     area;
    double  pos[2];
    int     coord_num;
    int     x_coord[AR_CHAIN_MAX];
    int     y_coord[AR_CHAIN_MAX];
    int     vertex[5];
} ARMarkerInfo2;

/** 
 * \struct arPrevInfo
 * \brief structure for temporal continuity of tracking
 *
 * History structure for arDetectMarkerLite and arGetTransMatCont
 */

typedef struct {
	ARMarkerInfo  marker;
	int     count;
} arPrevInfo;

/** \struct ARParam
* \brief camera intrinsic parameters.
* 
* This structure contains the main parameters for
* the intrinsic parameters of the camera
* representation. The camera used is a pinhole
* camera with standard parameters. User should
* consult a computer vision reference for more
* information. (e.g. Three-Dimensional Computer Vision 
* (Artificial Intelligence) by Olivier Faugeras).
* \param xsize length of the image (in pixels).
* \param ysize height of the image (in pixels).
* \param mat perspective matrix (K).
* \param dist_factor radial distortions factor
*          dist_factor[0]=x center of distortion
*          dist_factor[1]=y center of distortion
*          dist_factor[2]=distortion factor
*          dist_factor[3]=scale factor
*/
typedef struct {
    int      xsize, ysize;
    double   mat[3][4];
    double   dist_factor[4];
} ARParam;

typedef struct {
    int      xsize, ysize;
    double   matL[3][4];
    double   matR[3][4];
    double   matL2R[3][4];
    double   dist_factorL[4];
    double   dist_factorR[4];
} ARSParam;

typedef struct {
	int height;
	int width;
	ARUint8* frame;
	unsigned int format;
	unsigned int pix_size;
} ARFrame;

typedef struct {
	
	int				debug;
	int				thresh;

	ARMarkerInfo	*wmarker_info;
	ARMarkerInfo2	*wmarker_info2;
	ARParam			wparam;
	int				wmarker_num;
	int				wmarker_num2;

	ARFrame			image;

	arPrevInfo		*prev_info;
	int				prev_num;


	ARInt16			*l_image;

	unsigned short	pix_offset[3];

	artMarker		**markers;
	unsigned int	markercount;

        
} arToolkit;


#endif
