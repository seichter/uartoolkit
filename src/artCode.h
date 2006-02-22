#ifndef getcode_h
#define getcode_h

#include "artoolkit/ar.h"

int artGetCodeNew(arToolkit *state, int *x_coord, int *y_coord, int *vertex,
               int *code, int *dir, double *cf );

ARMarkerInfo *artGetMarkerInfo(arToolkit *state);

int arLoadPattNew(arToolkit* state, const char *filename);

#endif
