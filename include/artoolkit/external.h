#ifndef AREXTERNAL_H
#define AREXTERNAL_H


typedef struct
{
	int id;

	unsigned char type;
	unsigned char visible;

	double width;
	double center[2];

	double transform[16];

} artMarker;



#endif