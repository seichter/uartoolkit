#include "artoolkit/ar.h"

#include "artCode.h"
#include "artLabeling.h"

/*
int arSavePatt( ARUint8 *image, ARMarkerInfo *marker_info, char *filename )
{
    FILE      *fp;
    ARUint8   ext_pat[4][AR_PATT_SIZE_Y][AR_PATT_SIZE_X][3];
    int       vertex[4];
    int       i, j, k, x, y;

    for( i = 0; i < state->wmarker_num; i++ ) {
        if( marker_info->area   == marker_info2[i].area
         && marker_info->pos[0] == marker_info2[i].pos[0]
         && marker_info->pos[1] == marker_info2[i].pos[1] ) break;
    }
    if( i == state->wmarker_num ) return -1;

    for( j = 0; j < 4; j++ ) {
        for( k = 0; k < 4; k++ ) {
            vertex[k] = marker_info2[i].vertex[(k+j+2)%4];
        }
        arGetPatt( image, marker_info2[i].x_coord,
                   marker_info2[i].y_coord, vertex, ext_pat[j] );
    }

    fp = fopen( filename, "w" );
    if( fp == NULL ) return -1;

    for( i = 0; i < 4; i++ ) {
        for( j = 0; j < 3; j++ ) {
            for( y = 0; y < AR_PATT_SIZE_Y; y++ ) {
                for( x = 0; x < AR_PATT_SIZE_X; x++ ) {
                    fprintf( fp, "%4d", ext_pat[i][y][x][j] );
                }
                fprintf(fp, "\n");
            }
        }
        fprintf(fp, "\n");
    }

    fclose( fp );

    return 0;
}
*/

/*
int arDetectMarker(ARUint8 *dataPtr, int thresh,
                    ARMarkerInfo **marker_info, int *marker_num )
*/
int artDetect(arToolkit* state) {

    ARInt16                *limage;
    int                    label_num;
    int                    *area, *clip, *label_ref;
    double                 *pos;
    double                 rarea, rlen, rlenmin;
    double                 diff, diffmin;
    int                    cid, cdir;
    int                    i, j, k;

    limage = artLabeling(state, &label_num, &area, &pos, &clip, &label_ref );

    if (limage == 0) return -1;

	arDetectMarker2New( state, label_num, label_ref, area, pos,
		clip, AR_AREA_MAX, AR_AREA_MIN, 1.0);

	artGetMarkerInfo(state);

    for( i = 0; i < state->prev_num; i++ ) {

        rlenmin = 10.0;
        cid = -1;
        for( j = 0; j < state->wmarker_num; j++ ) {
            rarea = (double)state->prev_info[i].marker.area / (double)state->wmarker_info[j].area;
            if( rarea < 0.7 || rarea > 1.43 ) continue;
            rlen = ( (state->wmarker_info[j].pos[0] - state->prev_info[i].marker.pos[0])
                   * (state->wmarker_info[j].pos[0] - state->prev_info[i].marker.pos[0])
                   + (state->wmarker_info[j].pos[1] - state->prev_info[i].marker.pos[1])
                   * (state->wmarker_info[j].pos[1] - state->prev_info[i].marker.pos[1]) ) / state->wmarker_info[j].area;
            if( rlen < 0.5 && rlen < rlenmin ) {
                rlenmin = rlen;
                cid = j;
            }
        }
        if( cid >= 0 && state->wmarker_info[cid].cf < state->prev_info[i].marker.cf ) {
            state->wmarker_info[cid].cf = state->prev_info[i].marker.cf;
            state->wmarker_info[cid].id = state->prev_info[i].marker.id;
            diffmin = 10000.0 * 10000.0;
            cdir = -1;
            for( j = 0; j < 4; j++ ) {
                diff = 0;
                for( k = 0; k < 4; k++ ) {
                    diff += (state->prev_info[i].marker.vertex[k][0] - state->wmarker_info[cid].vertex[(j+k)%4][0])
                          * (state->prev_info[i].marker.vertex[k][0] - state->wmarker_info[cid].vertex[(j+k)%4][0])
                          + (state->prev_info[i].marker.vertex[k][1] - state->wmarker_info[cid].vertex[(j+k)%4][1])
                          * (state->prev_info[i].marker.vertex[k][1] - state->wmarker_info[cid].vertex[(j+k)%4][1]);
                }
                if( diff < diffmin ) {
                    diffmin = diff;
                    cdir = (state->prev_info[i].marker.dir - j + 4) % 4;
                }
            }
            state->wmarker_info[cid].dir = cdir;
        }
    }

    for (i = 0; i < state->wmarker_num; i++ ) {
        
		if( state->wmarker_info[i].cf < 0.5 ) {
			state->wmarker_info[i].id = -1;
		}
   }


/*------------------------------------------------------------*/

    for( i = j = 0; i < state->prev_num; i++ ) {
        state->prev_info[i].count++;
        if( state->prev_info[i].count < 4 ) {
            state->prev_info[j] = state->prev_info[i];
            j++;
        }
    }

    state->prev_num = j;

    for( i = 0; i < state->wmarker_num; i++ ) {
        if( state->wmarker_info[i].id < 0 ) continue;

        for( j = 0; j < state->prev_num; j++ ) {
            if( state->prev_info[j].marker.id == state->wmarker_info[i].id ) break;
        }
        state->prev_info[j].marker = state->wmarker_info[i];
        state->prev_info[j].count  = 1;
        if( j == state->prev_num ) state->prev_num++;
    }

    for( i = 0; i < state->prev_num; i++ ) {
        for( j = 0; j < state->wmarker_num; j++ ) {
            rarea = (double)state->prev_info[i].marker.area / (double)state->wmarker_info[j].area;
            if( rarea < 0.7 || rarea > 1.43 ) continue;
            rlen = ( (state->wmarker_info[j].pos[0] - state->prev_info[i].marker.pos[0])
                   * (state->wmarker_info[j].pos[0] - state->prev_info[i].marker.pos[0])
                   + (state->wmarker_info[j].pos[1] - state->prev_info[i].marker.pos[1])
                   * (state->wmarker_info[j].pos[1] - state->prev_info[i].marker.pos[1]) ) / state->wmarker_info[j].area;
            if( rlen < 0.5 ) break;
        }
        if( j == state->wmarker_num ) {
            state->wmarker_info[state->wmarker_num] = state->prev_info[i].marker;
            state->wmarker_num++;
        }
    }

	if (state->debug)
		for (i = 0; i < state->wmarker_num; i++) {
			printf("Detect: #%2d of %2d id:%d pos:(%6.3f,%6.3f)\tcf:%6.3f\n",
				i,
				state->wmarker_num,
				state->wmarker_info[i].id,
				state->wmarker_info[i].pos[0],state->wmarker_info[i].pos[1],
				state->wmarker_info[i].cf);
		};

	/* indicate how many markers have been found */
	return state->wmarker_num;
}

int artDetectLite(arToolkit *state) {

    ARInt16                *limage;
    int                    label_num;
    int                    *area, *clip, *label_ref;
    double                 *pos;
    int                    i;

    limage = artLabeling(state,&label_num, &area, &pos, &clip, &label_ref );
    
	if (limage == 0) return -1;

    arDetectMarker2New(state, label_num, label_ref,
                                    area, pos, clip, AR_AREA_MAX, AR_AREA_MIN,
                                    1.0);

    if (0 == state->wmarker_info2) return -1;

    artGetMarkerInfo(state);

    if( state->wmarker_info == 0 ) return -1;

    for( i = 0; i < state->wmarker_num; i++ ) 
	{

		if (state->wmarker_info[i].cf < 0.5 ) 
		{
			state->wmarker_info[i].id = -1;
		}
    }

    return 0;
}
