#ifndef ARTMATRIX_H
#define ARTMATRIX_H


#include "artoolkit/types.h"
#include "artoolkit/enums.h"
#include "artoolkit/external.h"
#include "artoolkit/param.h"


extern double artGetOpenGLMatrix(ARParam *param, 
						  ARMarkerInfo *marker_info,
						  double center[2], 
						  double width, 
						  double conv[3][4] );

#endif
