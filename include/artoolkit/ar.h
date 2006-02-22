/*  --------------------------------------------------------------------------
*   Copyright (c) 20042-2005 HIT Lab NZ.
*   The distribution policy is described in the file COPYING.txt furnished 
*    with this library.
*   -------------------------------------------------------------------------*/
/**
*  \file ar.h
*  \brief ARToolKit subroutines.
*
*  Core of the ARToolKit Library. This file provides image analysis and marker 
*  detection routines. Differents routines give access to camera and marker
*  configurations. Other routines manipulate marker info structures for 
*  deliver 3D transformation of markers (more precisely the position of the camera
*  in function of the marker coordinate system).
*	
*   \remark 
*
*   History :
*
*  \author Hirokazu Kato kato@sys.im.hiroshima-cu.ac.jp
*  \version 3.1
*  \date 01/12/07
**/
/*  --------------------------------------------------------------------------
*   History : 
*   Rev		Date		Who		Changes
*
*----------------------------------------------------------------------------*/

#ifndef AR_H
#define AR_H

// ============================================================================
//	Public includes.
// ============================================================================



#include "artoolkit/types.h"
#include <artoolkit/config.h>
#include <artoolkit/param.h>



extern int arDetectMarker2New( arToolkit *state, int label_num, int *label_ref,
                                int *warea, double *wpos, int *wclip,
                                int area_max, int area_min, double factor);

// ============================================================================
//	Public types and defines.
// ============================================================================

/** \def arMalloc(V,T,S)
* \brief allocation macro function
*
* allocate S element of type T.
* \param V returned allocated area pointer
* \param T type of element
* \param S number of element
*/
#define arMalloc(V,T,S)  \
{ if( ((V) = (T *)malloc( sizeof(T) * (S) )) == 0 ) \
{printf("malloc error!!\n"); exit(1);} }


// ============================================================================
//	Public globals.
// ============================================================================

/** \var int arDebug
* \brief activate artoolkit debug mode
*
* control debug informations in ARToolKit.
* the possible values are:
* - 0: not in debug mode
* - 1: in debug mode
* by default: 0 
*/
extern int      arDebug;

/** \var ARUint8 *arImage
* \brief internal image
*
* internal image used. (access only for debugging ARToolKit)
* by default: NULL
*/
extern ARUint8  *arImage;

/** \var int arFittingMode
* \brief fitting display mode use by ARToolkit.
*
* Correction mode for the distorsion of the camera.
* You can enable a correction with a texture mapping.
* the possible values are:
* - AR_FITTING_TO_INPUT: input image
* - AR_FITTING_TO_IDEAL: compensated image
* by default: DEFAULT_FITTING_MODE in config.h
*/
extern int      arFittingMode;

/** \var int arImageProcMode
* \brief define the image size mode for marker detection.
*
* Video image size for marker detection. This control
* if all the image is analyzed 
* the possible values are :
* - AR_IMAGE_PROC_IN_FULL: full image uses.
* - AR_IMAGE_PROC_IN_HALF: half image uses.
* by default: DEFAULT_IMAGE_PROC_MODE in config.h
*/
extern int      arImageProcMode;

/** \var ARParam arParam
* \brief internal intrinsic camera parameter
*
* internal variable for camera intrinsic parameters
*/
extern ARParam  arParam;

/** \var int arImXsize
* \brief internal image size in width.
*
* internal image size in width (generally initialize in arInitCparam)
*/
/** \var int arImYsize
* \brief internal image size in heigth
*
* internal image size in heigth (generally initialize in arInitCparam)
*/
extern int      arImXsize, arImYsize;

/** \var int arTemplateMatchingMode
* \brief XXXBK
*
* XXXBK
* the possible values are :
* AR_TEMPLATE_MATCHING_COLOR: Color Template
* AR_TEMPLATE_MATCHING_BW: BW Template
* by default: DEFAULT_TEMPLATE_MATCHING_MODE in config.h
*/
extern int      arTemplateMatchingMode;

/** \var int arMatchingPCAMode
* \brief XXXBK
*
* XXXBK
* the possible values are :
* -AR_MATCHING_WITHOUT_PCA: without PCA
* -AR_MATCHING_WITH_PCA: with PCA
* by default: DEFAULT_MATCHING_PCA_MODE in config.h
*/
extern int      arMatchingPCAMode;

// ============================================================================
//	Public functions.
// ============================================================================

/*
   Initialization
*/

/**
* \brief initialize camera parameters.
*
* set the camera parameters specified in the camera parameters structure 
* *param to static memory in the AR library. These camera parameters are  
* typically read from a data file at program startup. In the video-see through 
* AR applications, the default camera parameters are sufficient, no camera 
* calibration is needed.
* \param param the camera parameter structure
* \return always 0
*/
int arInitCparam( ARParam *param );

/**
* \brief load markers description from a file
*
* load the bitmap pattern specified in the file filename into the pattern
* matching array for later use by the marker detection routines.
* \param filename name of the file containing the pattern bitmap to be loaded
* \return the identity number of the pattern loaded or �1 if the pattern load failed.
*/
int arLoadPatt( const char *filename );

/*
   Detection
*/

/**
* \brief main function to detect the square markers in the video input frame.
*
* This function proceeds to thresholding, labeling, contour extraction and line corner estimation 
* (and maintains an history).
* It's one of the main function of the detection routine with arGetTransMat.
* \param dataPtr a pointer to the color image which is to be searched for square markers. 
*                The pixel format depend of your architecture. Generally ABGR, but the images 
*                are treated as a gray scale, so the order of BGR components does not matter. 
*                However the ordering of the alpha comp, A, is important.
* \param thresh  specifies the threshold value (between 0-255) to be used to convert
*                the input image into a binary image.
* \param marker_info a pointer to an array of ARMarkerInfo structures returned 
*                    which contain all the information about the detected squares in the image
* \param marker_num the number of detected markers in the image.
* \return 0 when the function completes normally, -1 otherwise
*/
int arDetectMarker( ARUint8 *dataPtr, int thresh,
                    ARMarkerInfo **marker_info, int *marker_num );

/**
* \brief main function to detect rapidly the square markers in the video input frame.
*
* this function is a simpler version of arDetectMarker that does not have the 
* same error correction functions and so runs a little faster, but is more error prone
* 
* \param dataPtr a pointer to the color image which is to be searched for square markers. 
*                The pixel format depend of your architecture. Generally ABGR, but the images 
*                are treated as a gray scale, so the order of BGR components does not matter. 
*                However the ordering of the alpha component, A, is important.
* \param thresh  specifies the threshold value (between 0-255) to be used to convert
*                the input image into a binary image.
* \param marker_info a pointer to an array of ARMarkerInfo structures returned 
*                    which contain all the information about the detected squares in the image
* \param marker_num the number of detected markers in the image.
* \return 0 when the function completes normally, -1 otherwise
*/
int arDetectMarkerLite( ARUint8 *dataPtr, int thresh,
                        ARMarkerInfo **marker_info, int *marker_num );

/**
* \brief compute camera position in function of detected markers.
*
* calculate the transformation between a detected marker and the real camera,
* i.e. the position and orientation of the camera relative to the tracking mark.
* \param marker_info the structure containing the parameters for the marker for 
*                    which the camera position and orientation is to be found relative to.
*                    This structure is found using arDetectMarker.
* \param center the physical center of the marker. arGetTransMat assumes that the marker 
*              is in x-y plane, and z axis is pointing downwards from marker plane. 
*              So vertex positions can be represented in 2D coordinates by ignoring the
*              z axis information. The marker vertices are specified in order of clockwise. 
* \param width the size of the marker (in mm). 
* \param conv the transformation matrix from the marker coordinates to camera coordinate frame, 
*             that is the relative position of real camera to the real marker
* \return always 0. 
*/

/*
double arGetTransMat( ARMarkerInfo *marker_info,
                      double center[2], double width, double conv[3][4] );

*/
/**
* \brief compute camera position in function of detected marker with an history function.
*
* calculate the transformation between a detected marker and the real camera,
* i.e. the position and orientation of the camera relative to the tracking mark. Since
* this routine operate on previous values, the result are more stable (less jittering).
* 
* \param marker_info the structure containing the parameters for the marker for 
*                    which the camera position and orientation is to be found relative to.
*                    This structure is found using arDetectMarker.
* \param prev_conv the previous transformation matrix obtain. 
* \param center the physical center of the marker. arGetTransMat assumes that the marker 
*              is in x-y plane, and z axis is pointing downwards from marker plane. 
*              So vertex positions can be represented in 2D coordinates by ignoring the
*              z axis information. The marker vertices are specified in order of clockwise. 
* \param width the size of the marker (in mm). 
* \param conv the transformation matrix from the marker coordinates to camera coordinate frame, 
*             that is the relative position of real camera to the real marker
* \return always 0. 
*/

/*
double arGetTransMatCont( ARMarkerInfo *marker_info, double prev_conv[3][4],
                          double center[2], double width, double conv[3][4] );

double arGetTransMat2( double rot[3][3], double pos2d[][2],
                       double pos3d[][2], int num, double conv[3][4] );
double arGetTransMat3( double rot[3][3], double ppos2d[][2],
                     double ppos3d[][2], int num, double conv[3][4],
                     double *dist_factor, double cpara[3][4] );
double arGetTransMat4( double rot[3][3], double ppos2d[][2],
                       double ppos3d[][3], int num, double conv[3][4] );
double arGetTransMat5( double rot[3][3], double ppos2d[][2],
                       double ppos3d[][3], int num, double conv[3][4],
                       double *dist_factor, double cpara[3][4] );
*/

/**
* \brief remove a pattern from memory.
*
* desactivate a pattern and remove from memory. post-condition
* of this function is unavailability of the pattern.
* \param patt_no number of pattern to free
* \return return 1 in success, -1 if error
*/
int arFreePatt( int patt_no );

/**
* \brief activate a pattern on the recognition procedure.
*
* Activate a pattern to be check during the template matching
* operation.
* \param patt_no number of pattern to activate
* \return return 1 in success, -1 if error
*/
int arActivatePatt( int pat_no );

/**
* \brief desactivate a pattern on the recognition procedure.
*
* Desactivate a pattern for not be check during the template matching
* operation.
* \param patt_no number of pattern to desactivate
* \return return 1 in success, -1 if error
*/
int arDeactivatePatt( int pat_no );

/**
* \brief save a marker.
*
* used in mk_patt to save a bitmap of the pattern of the currently detected marker.
* The saved image is a table of the normalized viewed pattern.
* \param image a pointer to the image containing the marker pattern to be trained.
* \param marker_info a pointer to the ARMarkerInfo structure of the pattern to be trained.
* \param filename The name of the file where the bitmap image is to be saved.
* \return 0 if the bitmap image is successfully saved, -1 otherwise. 
*/
int arSavePatt( ARUint8 *image,
                ARMarkerInfo *marker_info, char *filename );


/*
    Utility
*/

/**
* \brief Inverse a non-square matrix.
*
* Inverse a matrix in a non homogeneous format. The matrix
* need to be euclidian.
* \param s matrix input	
* \param d resulted inverse matrix.
* \return 0 if the inversion success, -1 otherwise
* \remark input matrix can be also output matrix
*/
int    arUtilMatInv( double s[3][4], double d[3][4] );

/**
* \brief Multiplication of two matrix.
*
* This procedure do a multiplication matrix between s1 and s2 and return
* the result in d : d=s1*s2. The precondition is the output matrix
* need to be different of input matrix. The precondition is euclidian matrix.
* \param s1 first matrix.
* \param s2 second matrix.
* \param d resulted multiplication matrix.
* \return 0 if the multiplication success, -1 otherwise
*/
int    arUtilMatMul( double s1[3][4], double s2[3][4], double d[3][4] );

/**
* \brief extract a quaternion/position of matrix.
*
* Extract a rotation (quaternion format) and a position (vector format) 
* from a transformation matrix. The precondition is an euclidian matrix.
* \param m source matrix
* \param q a rotation represented by a quaternion.
* \param p a translation represented by a vector.
* \return 0 if the extraction success, -1 otherwise (quaternion not normalize)
*/
int    arUtilMat2QuatPos( double m[3][4], double q[4], double p[3] );

/**
* \brief create a matrix with a quaternion/position.
*
* Create a transformation matrix from a quaternion rotation and a vector translation.
* \param q a rotation represented by a quaternion.
* \param p a translation represented by a vector.
* \param m destination matrix
* \return always 0
*/
int    arUtilQuatPos2Mat( double q[4], double p[3], double m[3][4] );

/**
* \brief get the time with the ARToolkit timer.
* 
* Give the time elapsed since the reset of the timer.
* \return elapsed time (in milliseconds)
*/
double arUtilTimer(void);

/**
* \brief reset the internal timer of ARToolkit.
*
* Reset the internal timer used by ARToolKit.
* timer measurement (with arUtilTimer()).
*/
void   arUtilTimerReset(void);

/**
* \brief sleep the actual thread.
*
* Sleep the actual thread.
* \param msec time to sleep (in millisecond)
*/
void   arUtilSleep( int msec );

/*
  Internal processing
*/

/**
* \brief extracted connected component from image.
*
* Labeling the input image, i.e. extracted connected component from the 
* inptu video image.
* \param image input image
* \param thresh lighting threshold
* \param label_num number of detected components
* \param area XXXBK
* \param pos XXXBK
* \param clip XXXBK
* \param label_ref XXXBK
* \return XXXBK
*/
ARInt16 *arLabeling( ARUint8 *image, int thresh,
                     int *label_num, int **area, double **pos, int **clip,
                     int **label_ref );

/**
* \brief  XXXBK
*
*  XXXBK
* \param num XXXBK
* \param area XXXBK
* \param clip XXXBK
* \param pos XXXBK
*/
void arGetImgFeature( int *num, int **area, int **clip, double **pos );

/**
* \brief   XXXBK
*
*   XXXBK
* \param limage  XXXBK
* \param label_num  XXXBK
* \param label_ref  XXXBK
* \param warea  XXXBK
* \param wpos  XXXBK
* \param wclip  XXXBK
* \param area_max  XXXBK
* \param area_min  XXXBK
* \param factor  XXXBK
* \param marker_num  XXXBK
* \return XXXBK  XXXBK
*/
ARMarkerInfo2 *arDetectMarker2( ARInt16 *limage,
                                int label_num, int *label_ref,
                                int *warea, double *wpos, int *wclip,
                                int area_max, int area_min, double factor, int *marker_num );

/**
* \brief information on 
*
*  XXXBK
* \param image XXXBK
* \param marker_info2 XXXBK
* \param marker_num XXXBK
* \return XXXBK
*/
ARMarkerInfo *arGetMarkerInfo( ARUint8 *image,
                               ARMarkerInfo2 *marker_info2, int *marker_num );

/**
* \brief  XXXBK
*
*  XXXBK
* \param image XXXBK
* \param x_coord XXXBK
* \param y_coord XXXBK
* \param vertex XXXBK
* \param code XXXBK
* \param dir XXXBK
* \param cf XXXBK
* \return XXXBK
*/
int arGetCode( ARUint8 *image, int *x_coord, int *y_coord, int *vertex,
               int *code, int *dir, double *cf );

/**
* \brief return a normalized pattern from a video image.
*
* This function return a normalized pattern from a video image. The
* format is a table with AR_PATT_SIZE_X by AR_PATT_SIZE_Y
* \param image video input image
* \param x_coord XXXBK
* \param y_coord XXXBK
* \param vertex XXXBK
* \param ext_pat detected pattern.
* \return  XXXBK
*/
int arGetPatt( ARUint8 *image, int *x_coord, int *y_coord, int *vertex,
               ARUint8 ext_pat[AR_PATT_SIZE_Y][AR_PATT_SIZE_X][3] );

/**
* \brief estimate a line from a list of point.
*
* Compute a linear regression from a list of point.
* \param x_coord X coordinate of points
* \param y_coord Y coordinate of points
* \param coord_num number of points
* \param vertex XXXBK 
* \param line XXXBK 
* \param v XXXBK
* \return  XXXBK
*/
int arGetLine(int x_coord[], int y_coord[], int coord_num,
              int vertex[], double line[4][3], double v[4][2]);

/**
* \brief  XXXBK
*
*  XXXBK
* \param limage XXXBK
* \param label_ref XXXBK
* \param label XXXBK
* \param clip XXXBK
* \param marker_info2 XXXBK
* \return  XXXBK
*/
int arGetContour( ARInt16 *limage, int *label_ref,
                  int label, int clip[4], ARMarkerInfo2 *marker_info2 );

/**
* \brief  XXXBK
*
*  XXXBK
* \param rot XXXBK
* \param trans XXXBK
* \param cpara XXXBK
* \param vertex XXXBK
* \param pos2d XXXBK
* \param num XXXBK
* \return XXXBK
*/
/*
double arModifyMatrix( double rot[3][3], double trans[3], double cpara[3][4],
                             double vertex[][3], double pos2d[][2], int num );
*/
/**
* \brief extract euler angle from a rotation matrix.
*
* Based on a matrix rotation representation, furnish the cprresponding euler angles.
* \param rot the initial rotation matrix
* \param wa XXXBK:which element ?
* \param wb XXXBK:which element ?
* \param wc XXXBK:which element ?
* \return XXXBK
*/
int arGetAngle( double rot[3][3], double *wa, double *wb, double *wc );

/**
* \brief create a rotation matrix with euler angle.
*
* Based on a euler description, furnish a rotation matrix.
* \param a XXXBK:which element ?
* \param b XXXBK:which element ?
* \param c XXXBK:which element ?
* \param rot the resulted rotation matrix
* \return XXXBK
*/
int arGetRot( double a, double b, double c, double rot[3][3] );

/**
* \brief XXXBK
*
* XXXBK
* \param a XXXBK
* \param b XXXBK
* \param c XXXBK
* \param trans XXXBK
* \param trans2 XXXBK
* \param cpara XXXBK
* \param ret XXXBK
* \return XXXBK
*/
int arGetNewMatrix( double a, double b, double c,
                    double trans[3], double trans2[3][4],
                    double cpara[3][4], double ret[3][4] );

/**
* \brief XXXBK
*
* XXXBK:initial of what ?
* \param marker_info XXXBK
* \param cpara XXXBK
* \param rot XXXBK
* \return XXXBK
*/
int arGetInitRot( ARMarkerInfo *marker_info, double cpara[3][4], double rot[3][3] );




/*------------------------------------*/

extern ARUint8  *arImageL;
extern ARUint8  *arImageR;
extern ARSParam arsParam;
extern double   arsMatR2L[3][4];

int           arsInitCparam      ( ARSParam *sparam );
void          arsGetImgFeature   ( int *num, int **area, int **clip, double **pos, int LorR );
ARInt16      *arsLabeling        ( ARUint8 *image, int thresh,
                                   int *label_num, int **area, double **pos, int **clip,
                                   int **label_ref, int LorR );
int           arsGetLine         ( int x_coord[], int y_coord[], int coord_num,
                                   int vertex[], double line[4][3], double v[4][2], int LorR);
ARMarkerInfo *arsGetMarkerInfo   ( ARUint8 *image,
                                   ARMarkerInfo2 *marker_info2, int *marker_num, int LorR );
int           arsDetectMarker    ( ARUint8 *dataPtr, int thresh,
                                   ARMarkerInfo **marker_info, int *marker_num, int LorR );
int           arsDetectMarkerLite( ARUint8 *dataPtr, int thresh,
                                   ARMarkerInfo **marker_info, int *marker_num, int LorR );
double        arsGetTransMat     ( ARMarkerInfo *marker_infoL, ARMarkerInfo *marker_infoR,
                                   double center[2], double width,
                                   double transL[3][4], double transR[3][4] );
double        arsGetTransMatCont ( ARMarkerInfo *marker_infoL, ARMarkerInfo *marker_infoR,
                                   double prev_conv[3][4],
                                   double center[2], double width,
                                   double transL[3][4], double transR[3][4] );
double        arsGetTransMat2    ( double rot[3][3],
                                   double ppos2dL[][2], double ppos3dL[][3], int numL,
                                   double ppos2dR[][2], double ppos3dR[][3], int numR,
                                   double transL[3][4], double transR[3][4] );
double        arsGetPosErr( double pos2dL[2], double pos2dR[2] );
int           arsCheckPosition   ( double pos2dL[2], double pos2dR[2], double thresh );
int           arsCheckMarkerPosition( ARMarkerInfo *marker_infoL, ARMarkerInfo *marker_infoR,
                                      double thresh );

double arsModifyMatrix( double rot[3][3], double trans[3], ARSParam *arsParam,
                        double pos3dL[][3], double pos2dL[][2], int numL,
                        double pos3dR[][3], double pos2dR[][2], int numR );

#ifdef __cplusplus
}
#endif
#endif
