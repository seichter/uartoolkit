/*******************************************************
 *
 * Author: Hirokazu Kato
 *
 *         kato@sys.im.hiroshima-cu.ac.jp
 *
 * Revision: 3.1
 * Date: 01/12/07
 *
 *
 * modified by Thomas Pintaric [tp], Vienna University of Technology
 * modified by Hartmut Seichter [hs], HITLab NZ
 **********************************************************************/

#include "artLabeling.h"

#ifdef _WIN32
#  include <windows.h>
#  define put_zero(p,s) ZeroMemory(p, s)
#else
#  include <string.h>
#  define put_zero(p,s) memset((void *)p, 0, s)
#endif

#define USE_OPTIMIZATIONS 1
#define WORK_SIZE   1024*32

/*****************************************************************************/
// BUG in ARToolkit 2.65
// Hardcoded buffer (600*500) is too small for full-size DV-PAL/NTSC resolutions of
// 720x576 and 720x480, respectively. Results in segment faults.
/*
static ARInt16      l_imageL[640*500];
static ARInt16      l_imageR[640*500];
*/

#define HARDCODED_BUFFER_WIDTH  1024
#define HARDCODED_BUFFER_HEIGHT 1024

// static ARInt16      l_imageL[HARDCODED_BUFFER_WIDTH*HARDCODED_BUFFER_HEIGHT];
// static ARInt16      l_imageR[HARDCODED_BUFFER_WIDTH*HARDCODED_BUFFER_HEIGHT];
/*****************************************************************************/


/* static int          workL[WORK_SIZE];*/
static int          workR[WORK_SIZE];
/* static int          work2L[WORK_SIZE*7]; */
static int          work2R[WORK_SIZE*7];

/* static int          wlabel_numL; */
static int          wlabel_numR;
/* static int          wareaL[WORK_SIZE]; */
static int          wareaR[WORK_SIZE];
/* static int          wclipL[WORK_SIZE*4]; */
static int          wclipR[WORK_SIZE*4];
/* static double       wposL[WORK_SIZE*2]; */
static double       wposR[WORK_SIZE*2];

static unsigned char artBelowThresh(ARUint8 *pnt,int thresh, int format);

unsigned char 
artBelowThresh(ARUint8 *pnt,int thresh, int format) {
	switch(format) {
		case ART_PIXFORMAT_ARGB :
		case ART_PIXFORMAT_ABGR :
			return (*(pnt+1) + *(pnt+2) + *(pnt+3) <= thresh);
		case ART_PIXFORMAT_RGB :
		case ART_PIXFORMAT_BGR :
		case ART_PIXFORMAT_BGRA :
		case ART_PIXFORMAT_RGBA :
			return (*pnt + *(pnt+1) + *(pnt+2) <= thresh);
		case ART_PIXFORMAT_2vuy :
			return ( *(pnt+1) * 3 <= thresh );
		case ART_PIXFORMAT_yuvs :
			return ( *(pnt+0) * 3 <= thresh );
		default:
			printf("Unkown Image format %d",format);
			break;
	};			
	return 0;
};

typedef struct PixelOffset {
	unsigned short offset[3];
} PixelOffset;

PixelOffset PixelOffsets[] = {
	{{1,2,3}}, /* ART_PIXFORMAT_ARGB */ 
	{{1,2,3}}, /* ART_PIXFORMAT_ABGR */
	{{0,1,2}}, /* ART_PIXFORMAT_RGB */
	{{0,1,2}}, /* ART_PIXFORMAT_BGR */
	{{0,1,2}}, /* ART_PIXFORMAT_BGRA */
	{{0,1,2}}, /* ART_PIXFORMAT_RGBA */
	{{1,1,1}}, /* ART_PIXFORMAT_2vuy */
	{{0,0,0}}  /* ART_PIXFORMAT_yuvs */
};



/*
void arGetImgFeature( int *num, int **area, int **clip, double **pos )
{
    *num  = wlabel_numL;
    *area = wareaL;
    *clip = wclipL;
    *pos  = wposL;

    return;
}

*/

ARInt16 *artLabeling(arToolkit* state, 
						  int *label_num, int **area, double **pos, int **clip,
						  int **label_ref)
{

	ARUint8 *image = state->image.frame;
	int thresh = state->thresh;

	register int       i,j,k;                    /*  for loop            */
    
    register ARUint8   *pnt;                    /*  image pointer       */
    register ARInt16   *pnt1, *pnt2;            /*  image pointer       */
    int       *wk;								/*  pointer for work    */
    int       wk_max;							/*  work                */
    int       m,n;								 /*  work                */
    int       lxsize, lysize;
    int       poff;
    int       *work, *work2;
    int       *wlabel_num;
    int       *warea;
    int       *wclip;
    double    *wpos;

	ARInt16   *l_image = &state->l_image[0];

	unsigned short *pix_off = PixelOffsets[state->image.format].offset;

	int dbl_poff;

#ifdef USE_OPTIMIZATIONS
	int		  pnt2_index;   // [tp]
#endif

	

	/* thresh * 3 */
	thresh += thresh + thresh;

    work    = &workR[0];
    work2   = &work2R[0];
    wlabel_num = &wlabel_numR;
    warea   = &wareaR[0];
    wclip   = &wclipR[0];
    wpos    = &wposR[0];


    if( arImageProcMode == AR_IMAGE_PROC_IN_HALF ) {
    
    	printf("Half Mode");
    	
        lxsize = arImXsize / 2;
        lysize = arImYsize / 2;
    }
    else {
        lxsize = arImXsize;
        lysize = arImYsize;
    }

    pnt1 = &l_image[0];
    pnt2 = &l_image[(lysize-1)*lxsize];
    

#ifndef USE_OPTIMIZATIONS
	for(i = 0; i < lxsize; i++) {
        *(pnt1++) = *(pnt2++) = 0;
    }
#else
// 4x loop unrolling
	for(i = 0; i < lxsize-(lxsize % 4); i += 4) {
        *(pnt1++) = *(pnt2++) = 0;
        *(pnt1++) = *(pnt2++) = 0;
        *(pnt1++) = *(pnt2++) = 0;
        *(pnt1++) = *(pnt2++) = 0;
    }
#endif
    /*
    pnt1 = &l_image[0];
    pnt2 = &l_image[lxsize-1];
    */
    
    pnt1 = l_image;
    pnt2 = l_image + (lxsize - 1);

#ifndef USE_OPTIMIZATIONS
    for(i = 0; i < lysize; i++) {
        *pnt1 = *pnt2 = 0;
        pnt1 += lxsize;
        pnt2 += lxsize;
    }
#else
// 4x loop unrolling
    for(i = 0; i < lysize-(lysize%4); i+=4) {
		*pnt1 = *pnt2 = 0;
        pnt1 += lxsize;
        pnt2 += lxsize;

		*pnt1 = *pnt2 = 0;
        pnt1 += lxsize;
        pnt2 += lxsize;

		*pnt1 = *pnt2 = 0;
        pnt1 += lxsize;
        pnt2 += lxsize;

		*pnt1 = *pnt2 = 0;
        pnt1 += lxsize;
        pnt2 += lxsize;
    }
#endif

    wk_max = 0;
    pnt2 = &(l_image[lxsize+1]);
    if( arImageProcMode == AR_IMAGE_PROC_IN_HALF ) {
		pnt = &(image[(arImXsize*2+2) * state->image.pix_size]);
        poff = state->image.pix_size * 2;
    }
    else {
        pnt = &(image[(arImXsize+1) * state->image.pix_size]);
        poff = state->image.pix_size;
    }

	dbl_poff = poff + poff;

    for(j = 1; j < lysize-1; j++, pnt += dbl_poff, pnt2 += 2) 
	{
        for(i = 1; i < lxsize-1; i++, pnt += poff, pnt2++) 
		{
			if ((*(pnt + pix_off[0]) + *(pnt + pix_off[1]) + *(pnt + pix_off[2])) < thresh)
			{
                pnt1 = &(pnt2[-lxsize]);
                if( *pnt1 > 0 ) {
                    *pnt2 = *pnt1;

#ifndef USE_OPTIMIZATIONS
					// ORIGINAL CODE
					work2[((*pnt2)-1)*7+0] ++;
                    work2[((*pnt2)-1)*7+1] += i;
                    work2[((*pnt2)-1)*7+2] += j;
                    work2[((*pnt2)-1)*7+6] = j;
#else
					// OPTIMIZED CODE [tp]
					// ((*pnt2)-1)*7 should be treated as constant, since
					//  work2[n] (n=0..xsize*ysize) cannot overwrite (*pnt2)
					pnt2_index = ((*pnt2) -1) * 7;
                    
					work2[pnt2_index+0]++;
                    work2[pnt2_index+1]+= i;
                    work2[pnt2_index+2]+= j;
                    work2[pnt2_index+6] = j;
					// --------------------------------
#endif
                }
                else if( *(pnt1+1) > 0 ) {
                    if( *(pnt1-1) > 0 ) {
                        m = work[*(pnt1+1)-1];
                        n = work[*(pnt1-1)-1];
                        if( m > n ) {
                            *pnt2 = n;
                            wk = &(work[0]);
                            for(k = 0; k < wk_max; k++) {
                                if( *wk == m ) *wk = n;
                                wk++;
                            }
                        }
                        else if( m < n ) {
                            *pnt2 = m;
                            wk = &(work[0]);
                            for(k = 0; k < wk_max; k++) {
                                if( *wk == n ) *wk = m;
                                wk++;
                            }
                        }

                        else *pnt2 = m;

#ifndef USE_OPTIMIZATIONS
						// ORIGINAL CODE
						work2[((*pnt2)-1)*7+0] ++;
                        work2[((*pnt2)-1)*7+1] += i;
                        work2[((*pnt2)-1)*7+2] += j;
                        work2[((*pnt2)-1)*7+6] = j;
#else
						// PERFORMANCE OPTIMIZATION:
						pnt2_index = ((*pnt2)-1) * 7;
						work2[pnt2_index+0]++;
						work2[pnt2_index+1]+= i;
						work2[pnt2_index+2]+= j;
						work2[pnt2_index+6] = j;
#endif

                    }
                    else if( *(pnt2-1) > 0 ) {
                        m = work[*(pnt1+1)-1];
                        n = work[*(pnt2-1)-1];
                        if( m > n ) {
                            *pnt2 = n;
                            wk = &(work[0]);
                            for(k = 0; k < wk_max; k++) {
                                if( *wk == m ) *wk = n;
                                wk++;
                            }
                        }
                        else if( m < n ) {
                            *pnt2 = m;
                            wk = &(work[0]);
                            for(k = 0; k < wk_max; k++) {
                                if( *wk == n ) *wk = m;
                                wk++;
                            }
                        }
                        else *pnt2 = m;

#ifndef USE_OPTIMIZATIONS
						// ORIGINAL CODE
                        work2[((*pnt2)-1)*7+0] ++;
                        work2[((*pnt2)-1)*7+1] += i;
                        work2[((*pnt2)-1)*7+2] += j;
#else
						// PERFORMANCE OPTIMIZATION:
						pnt2_index = ((*pnt2)-1) * 7;
						work2[pnt2_index+0]++;
						work2[pnt2_index+1]+= i;
						work2[pnt2_index+2]+= j;
#endif

                    }
                    else {
                        *pnt2 = *(pnt1+1);

#ifndef USE_OPTIMIZATIONS
						// ORIGINAL CODE
                        work2[((*pnt2)-1)*7+0] ++;
                        work2[((*pnt2)-1)*7+1] += i;
                        work2[((*pnt2)-1)*7+2] += j;
                        if( work2[((*pnt2)-1)*7+3] > i ) work2[((*pnt2)-1)*7+3] = i;
                        work2[((*pnt2)-1)*7+6] = j;
#else
						// PERFORMANCE OPTIMIZATION:
						pnt2_index = ((*pnt2)-1) * 7;
						work2[pnt2_index+0]++;
						work2[pnt2_index+1]+= i;
						work2[pnt2_index+2]+= j;
                        if( work2[pnt2_index+3] > i ) work2[pnt2_index+3] = i;
						work2[pnt2_index+6] = j;
#endif
                    }
                }
                else if( *(pnt1-1) > 0 ) {
                    *pnt2 = *(pnt1-1);

#ifndef USE_OPTIMIZATIONS
						// ORIGINAL CODE
                    work2[((*pnt2)-1)*7+0] ++;
                    work2[((*pnt2)-1)*7+1] += i;
                    work2[((*pnt2)-1)*7+2] += j;
                    if( work2[((*pnt2)-1)*7+4] < i ) work2[((*pnt2)-1)*7+4] = i;
                    work2[((*pnt2)-1)*7+6] = j;
#else
					// PERFORMANCE OPTIMIZATION:
					pnt2_index = ((*pnt2)-1) * 7;
					work2[pnt2_index+0]++;
					work2[pnt2_index+1]+= i;
					work2[pnt2_index+2]+= j;
                    if( work2[pnt2_index+4] < i ) work2[pnt2_index+4] = i;
					work2[pnt2_index+6] = j;
#endif
                }
                else if( *(pnt2-1) > 0) {
                    *pnt2 = *(pnt2-1);

#ifndef USE_OPTIMIZATIONS
						// ORIGINAL CODE
                    work2[((*pnt2)-1)*7+0] ++;
                    work2[((*pnt2)-1)*7+1] += i;
                    work2[((*pnt2)-1)*7+2] += j;
                    if( work2[((*pnt2)-1)*7+4] < i ) work2[((*pnt2)-1)*7+4] = i;
#else
					// PERFORMANCE OPTIMIZATION:
					pnt2_index = ((*pnt2)-1) * 7;
					work2[pnt2_index+0]++;
					work2[pnt2_index+1]+= i;
					work2[pnt2_index+2]+= j;
                    if( work2[pnt2_index+4] < i ) work2[pnt2_index+4] = i;
#endif
				}
                else {
                    wk_max++;
                    if( wk_max > WORK_SIZE ) {
                        return(0);
                    }
                    work[wk_max-1] = *pnt2 = wk_max;
                    work2[(wk_max-1)*7+0] = 1;
                    work2[(wk_max-1)*7+1] = i;
                    work2[(wk_max-1)*7+2] = j;
                    work2[(wk_max-1)*7+3] = i;
                    work2[(wk_max-1)*7+4] = i;
                    work2[(wk_max-1)*7+5] = j;
                    work2[(wk_max-1)*7+6] = j;
                }
            } else {
                *pnt2 = 0;
            }
        }
        if( arImageProcMode == AR_IMAGE_PROC_IN_HALF ) pnt += arImXsize * state->image.pix_size;
    }

    j = 1;
    wk = &(work[0]);
    for(i = 1; i <= wk_max; i++, wk++) {
        *wk = (*wk==i) ? j++ : work[(*wk)-1];
    }
    *label_num = *wlabel_num = j - 1;
    if( *label_num == 0 ) {
        return( l_image );
    }

    put_zero( (ARUint8 *)warea, *label_num *     sizeof(int) );
    put_zero( (ARUint8 *)wpos,  *label_num * 2 * sizeof(double) );

    for(i = 0; i < *label_num; i++) {
        wclip[i*4+0] = lxsize;
        wclip[i*4+1] = 0;
        wclip[i*4+2] = lysize;
        wclip[i*4+3] = 0;
    }
    for(i = 0; i < wk_max; i++) {
        j = work[i] - 1;
        warea[j]    += work2[i*7+0];
        wpos[j*2+0] += work2[i*7+1];
        wpos[j*2+1] += work2[i*7+2];
        if( wclip[j*4+0] > work2[i*7+3] ) wclip[j*4+0] = work2[i*7+3];
        if( wclip[j*4+1] < work2[i*7+4] ) wclip[j*4+1] = work2[i*7+4];
        if( wclip[j*4+2] > work2[i*7+5] ) wclip[j*4+2] = work2[i*7+5];
        if( wclip[j*4+3] < work2[i*7+6] ) wclip[j*4+3] = work2[i*7+6];
    }

    for( i = 0; i < *label_num; i++ ) {
        wpos[i*2+0] /= warea[i];
        wpos[i*2+1] /= warea[i];
    }

    *label_ref = work;
    *area      = warea;
    *pos       = wpos;
    *clip      = wclip;

    return (l_image);
}
 
