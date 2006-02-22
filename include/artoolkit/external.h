#ifndef AREXTERNAL_H
#define AREXTERNAL_H


typedef struct
{
	int id;

	unsigned char type;
	unsigned char visible;

	double width;
	double center[2];
	
	double pos[2];

	double transform[3][4];

} artMarker;



#endif

