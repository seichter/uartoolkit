#ifndef labeling_h
#define labeling_h


#include "artoolkit/sysconfig.h"
#include "artoolkit/enums.h"
#include "artoolkit/ar.h"


extern ARInt16 *artLabeling(arToolkit* state,
					int *label_num, int **area, double **pos, int **clip,
                    int **label_ref );


#endif