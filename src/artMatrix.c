#include "artMatrix.h"

#include "artoolkit/enums.h"
#include "artoolkit/types.h"
#include "artoolkit/param.h"
#include "artoolkit/matrix.h"
#include "artoolkit/ar.h"

#define P_MAX 500

static double  pos2d[P_MAX][2];
static double  pos3d[P_MAX][3];

#define MD_PI         3.14159265358979323846 

static double arGetTransMatSub( double rot[3][3], double ppos2d[][2],
                                double pos3d[][3], int num, double conv[3][4],
                                double *dist_factor, double cpara[3][4] );


static double arModifyMatrix( double rot[3][3], double trans[3], double cpara[3][4],
                             double vertex[][3], double pos2d[][2], int num );


static double arGetTransMat3( double rot[3][3], double ppos2d[][2],
                       double ppos3d[][2], int num, double conv[3][4],
                       double *dist_factor, double cpara[3][4] );



static int check_rotation( double rot[2][3] )
{
    double  v1[3], v2[3], v3[3];
    double  ca, cb, k1, k2, k3, k4;
    double  a, b, c, d;
    double  p1, q1, r1;
    double  p2, q2, r2;
    double  p3, q3, r3;
    double  p4, q4, r4;
    double  w;
    double  e1, e2, e3, e4;
    int     f;

    v1[0] = rot[0][0];
    v1[1] = rot[0][1];
    v1[2] = rot[0][2];
    v2[0] = rot[1][0];
    v2[1] = rot[1][1];
    v2[2] = rot[1][2];
    v3[0] = v1[1]*v2[2] - v1[2]*v2[1];
    v3[1] = v1[2]*v2[0] - v1[0]*v2[2];
    v3[2] = v1[0]*v2[1] - v1[1]*v2[0];
    w = sqrt( v3[0]*v3[0]+v3[1]*v3[1]+v3[2]*v3[2] );
    if( w == 0.0 ) return -1;
    v3[0] /= w;
    v3[1] /= w;
    v3[2] /= w;

    cb = v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
    if( cb < 0 ) cb *= -1.0;
    ca = (sqrt(cb+1.0) + sqrt(1.0-cb)) * 0.5;

    if( v3[1]*v1[0] - v1[1]*v3[0] != 0.0 ) {
        f = 0;
    }
    else {
        if( v3[2]*v1[0] - v1[2]*v3[0] != 0.0 ) {
            w = v1[1]; v1[1] = v1[2]; v1[2] = w;
            w = v3[1]; v3[1] = v3[2]; v3[2] = w;
            f = 1;
        }
        else {
            w = v1[0]; v1[0] = v1[2]; v1[2] = w;
            w = v3[0]; v3[0] = v3[2]; v3[2] = w;
            f = 2;
        }
    }
    if( v3[1]*v1[0] - v1[1]*v3[0] == 0.0 ) return -1;
    k1 = (v1[1]*v3[2] - v3[1]*v1[2]) / (v3[1]*v1[0] - v1[1]*v3[0]);
    k2 = (v3[1] * ca) / (v3[1]*v1[0] - v1[1]*v3[0]);
    k3 = (v1[0]*v3[2] - v3[0]*v1[2]) / (v3[0]*v1[1] - v1[0]*v3[1]);
    k4 = (v3[0] * ca) / (v3[0]*v1[1] - v1[0]*v3[1]);

    a = k1*k1 + k3*k3 + 1;
    b = k1*k2 + k3*k4;
    c = k2*k2 + k4*k4 - 1;

    d = b*b - a*c;
    if( d < 0 ) return -1;
    r1 = (-b + sqrt(d))/a;
    p1 = k1*r1 + k2;
    q1 = k3*r1 + k4;
    r2 = (-b - sqrt(d))/a;
    p2 = k1*r2 + k2;
    q2 = k3*r2 + k4;
    if( f == 1 ) {
        w = q1; q1 = r1; r1 = w;
        w = q2; q2 = r2; r2 = w;
        w = v1[1]; v1[1] = v1[2]; v1[2] = w;
        w = v3[1]; v3[1] = v3[2]; v3[2] = w;
        f = 0;
    }
    if( f == 2 ) {
        w = p1; p1 = r1; r1 = w;
        w = p2; p2 = r2; r2 = w;
        w = v1[0]; v1[0] = v1[2]; v1[2] = w;
        w = v3[0]; v3[0] = v3[2]; v3[2] = w;
        f = 0;
    }

    if( v3[1]*v2[0] - v2[1]*v3[0] != 0.0 ) {
        f = 0;
    }
    else {
        if( v3[2]*v2[0] - v2[2]*v3[0] != 0.0 ) {
            w = v2[1]; v2[1] = v2[2]; v2[2] = w;
            w = v3[1]; v3[1] = v3[2]; v3[2] = w;
            f = 1;
        }
        else {
            w = v2[0]; v2[0] = v2[2]; v2[2] = w;
            w = v3[0]; v3[0] = v3[2]; v3[2] = w;
            f = 2;
        }
    }
    if( v3[1]*v2[0] - v2[1]*v3[0] == 0.0 ) return -1;
    k1 = (v2[1]*v3[2] - v3[1]*v2[2]) / (v3[1]*v2[0] - v2[1]*v3[0]);
    k2 = (v3[1] * ca) / (v3[1]*v2[0] - v2[1]*v3[0]);
    k3 = (v2[0]*v3[2] - v3[0]*v2[2]) / (v3[0]*v2[1] - v2[0]*v3[1]);
    k4 = (v3[0] * ca) / (v3[0]*v2[1] - v2[0]*v3[1]);

    a = k1*k1 + k3*k3 + 1;
    b = k1*k2 + k3*k4;
    c = k2*k2 + k4*k4 - 1;

    d = b*b - a*c;
    if( d < 0 ) return -1;
    r3 = (-b + sqrt(d))/a;
    p3 = k1*r3 + k2;
    q3 = k3*r3 + k4;
    r4 = (-b - sqrt(d))/a;
    p4 = k1*r4 + k2;
    q4 = k3*r4 + k4;
    if( f == 1 ) {
        w = q3; q3 = r3; r3 = w;
        w = q4; q4 = r4; r4 = w;
        w = v2[1]; v2[1] = v2[2]; v2[2] = w;
        w = v3[1]; v3[1] = v3[2]; v3[2] = w;
        f = 0;
    }
    if( f == 2 ) {
        w = p3; p3 = r3; r3 = w;
        w = p4; p4 = r4; r4 = w;
        w = v2[0]; v2[0] = v2[2]; v2[2] = w;
        w = v3[0]; v3[0] = v3[2]; v3[2] = w;
        f = 0;
    }

    e1 = p1*p3+q1*q3+r1*r3; if( e1 < 0 ) e1 = -e1;
    e2 = p1*p4+q1*q4+r1*r4; if( e2 < 0 ) e2 = -e2;
    e3 = p2*p3+q2*q3+r2*r3; if( e3 < 0 ) e3 = -e3;
    e4 = p2*p4+q2*q4+r2*r4; if( e4 < 0 ) e4 = -e4;
    if( e1 < e2 ) {
        if( e1 < e3 ) {
            if( e1 < e4 ) {
                rot[0][0] = p1;
                rot[0][1] = q1;
                rot[0][2] = r1;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            }
            else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
        else {
            if( e3 < e4 ) {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            }
            else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
    }
    else {
        if( e2 < e3 ) {
            if( e2 < e4 ) {
                rot[0][0] = p1;
                rot[0][1] = q1;
                rot[0][2] = r1;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
            else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
        else {
            if( e3 < e4 ) {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p3;
                rot[1][1] = q3;
                rot[1][2] = r3;
            }
            else {
                rot[0][0] = p2;
                rot[0][1] = q2;
                rot[0][2] = r2;
                rot[1][0] = p4;
                rot[1][1] = q4;
                rot[1][2] = r4;
            }
        }
    }

    return 0;
}





static int check_dir( double dir[3], double st[2], double ed[2],
                      double cpara[3][4] )
{
    ARMat     *mat_a;
    double    world[2][3];
    double    camera[2][2];
    double    v[2][2];
    double    h;
    int       i, j;

    mat_a = arMatrixAlloc( 3, 3 );
    for(j=0;j<3;j++) for(i=0;i<3;i++) mat_a->m[j*3+i] = cpara[j][i];
    arMatrixSelfInv( mat_a );
    world[0][0] = mat_a->m[0]*st[0]*10.0
                + mat_a->m[1]*st[1]*10.0
                + mat_a->m[2]*10.0;
    world[0][1] = mat_a->m[3]*st[0]*10.0
                + mat_a->m[4]*st[1]*10.0
                + mat_a->m[5]*10.0;
    world[0][2] = mat_a->m[6]*st[0]*10.0
                + mat_a->m[7]*st[1]*10.0
                + mat_a->m[8]*10.0;
    arMatrixFree( mat_a );
    world[1][0] = world[0][0] + dir[0];
    world[1][1] = world[0][1] + dir[1];
    world[1][2] = world[0][2] + dir[2];

    for( i = 0; i < 2; i++ ) {
        h = cpara[2][0] * world[i][0]
          + cpara[2][1] * world[i][1]
          + cpara[2][2] * world[i][2];
        if( h == 0.0 ) return -1;
        camera[i][0] = (cpara[0][0] * world[i][0]
                      + cpara[0][1] * world[i][1]
                      + cpara[0][2] * world[i][2]) / h;
        camera[i][1] = (cpara[1][0] * world[i][0]
                      + cpara[1][1] * world[i][1]
                      + cpara[1][2] * world[i][2]) / h;
    }

    v[0][0] = ed[0] - st[0];
    v[0][1] = ed[1] - st[1];
    v[1][0] = camera[1][0] - camera[0][0];
    v[1][1] = camera[1][1] - camera[0][1];

    if( v[0][0]*v[1][0] + v[0][1]*v[1][1] < 0 ) {
        dir[0] = -dir[0];
        dir[1] = -dir[1];
        dir[2] = -dir[2];
    }

    return 0;
} 

int arGetInitRot( ARMarkerInfo *marker_info, double cpara[3][4], double rot[3][3] )
{
    double  wdir[3][3];
    double  w, w1, w2, w3;
    int     dir;
    int     j;

    dir = marker_info->dir;

    for( j = 0; j < 2; j++ ) {
        w1 = marker_info->line[(4-dir+j)%4][0] * marker_info->line[(6-dir+j)%4][1]
           - marker_info->line[(6-dir+j)%4][0] * marker_info->line[(4-dir+j)%4][1];
        w2 = marker_info->line[(4-dir+j)%4][1] * marker_info->line[(6-dir+j)%4][2]
           - marker_info->line[(6-dir+j)%4][1] * marker_info->line[(4-dir+j)%4][2];
        w3 = marker_info->line[(4-dir+j)%4][2] * marker_info->line[(6-dir+j)%4][0]
           - marker_info->line[(6-dir+j)%4][2] * marker_info->line[(4-dir+j)%4][0];

        wdir[j][0] =  w1*(cpara[0][1]*cpara[1][2]-cpara[0][2]*cpara[1][1])
                   +  w2*cpara[1][1]
                   -  w3*cpara[0][1];
        wdir[j][1] = -w1*cpara[0][0]*cpara[1][2]
                   +  w3*cpara[0][0];
        wdir[j][2] =  w1*cpara[0][0]*cpara[1][1];
        w = sqrt( wdir[j][0]*wdir[j][0]
                + wdir[j][1]*wdir[j][1]
                + wdir[j][2]*wdir[j][2] );
        wdir[j][0] /= w;
        wdir[j][1] /= w;
        wdir[j][2] /= w;
    }

    if( check_dir(wdir[0], marker_info->vertex[(4-dir)%4],
                  marker_info->vertex[(5-dir)%4], cpara) < 0 ) return -1;
    if( check_dir(wdir[1], marker_info->vertex[(7-dir)%4],
                  marker_info->vertex[(4-dir)%4], cpara) < 0 ) return -1;
    if( check_rotation(wdir) < 0 ) return -1;

    wdir[2][0] = wdir[0][1]*wdir[1][2] - wdir[0][2]*wdir[1][1];
    wdir[2][1] = wdir[0][2]*wdir[1][0] - wdir[0][0]*wdir[1][2];
    wdir[2][2] = wdir[0][0]*wdir[1][1] - wdir[0][1]*wdir[1][0];
    w = sqrt( wdir[2][0]*wdir[2][0]
            + wdir[2][1]*wdir[2][1]
            + wdir[2][2]*wdir[2][2] );
    wdir[2][0] /= w;
    wdir[2][1] /= w;
    wdir[2][2] /= w;
/*
    if( wdir[2][2] < 0 ) {
        wdir[2][0] /= -w;
        wdir[2][1] /= -w;
        wdir[2][2] /= -w;
    }
    else {
        wdir[2][0] /= w;
        wdir[2][1] /= w;
        wdir[2][2] /= w;
    }
*/

    rot[0][0] = wdir[0][0];
    rot[1][0] = wdir[0][1];
    rot[2][0] = wdir[0][2];
    rot[0][1] = wdir[1][0];
    rot[1][1] = wdir[1][1];
    rot[2][1] = wdir[1][2];
    rot[0][2] = wdir[2][0];
    rot[1][2] = wdir[2][1];
    rot[2][2] = wdir[2][2];

    return 0;
} 

int arGetRot( double a, double b, double c, double rot[3][3] )
{
    double   sina, sinb, sinc;
    double   cosa, cosb, cosc;
#if CHECK_CALC
    double   w[3];
    int      i;
#endif

    sina = sin(a); cosa = cos(a);
    sinb = sin(b); cosb = cos(b);
    sinc = sin(c); cosc = cos(c);
    rot[0][0] = cosa*cosa*cosb*cosc+sina*sina*cosc+sina*cosa*cosb*sinc-sina*cosa*sinc;
    rot[0][1] = -cosa*cosa*cosb*sinc-sina*sina*sinc+sina*cosa*cosb*cosc-sina*cosa*cosc;
    rot[0][2] = cosa*sinb;
    rot[1][0] = sina*cosa*cosb*cosc-sina*cosa*cosc+sina*sina*cosb*sinc+cosa*cosa*sinc;
    rot[1][1] = -sina*cosa*cosb*sinc+sina*cosa*sinc+sina*sina*cosb*cosc+cosa*cosa*cosc;
    rot[1][2] = sina*sinb;
    rot[2][0] = -cosa*sinb*cosc-sina*sinb*sinc;
    rot[2][1] = cosa*sinb*sinc-sina*sinb*cosc;
    rot[2][2] = cosb;

#if CHECK_CALC
    for(i=0;i<3;i++) w[i] = rot[i][2];
    for(i=0;i<3;i++) rot[i][2] = rot[i][1];
    for(i=0;i<3;i++) rot[i][1] = rot[i][0];
    for(i=0;i<3;i++) rot[i][0] = w[i];
#endif

    return 0;
} 


double artGetOpenGLMatrix(ARParam *param,ARMarkerInfo *marker_info,
                      double center[2], double width, double conv[3][4] )
{



    double  rot[3][3];
    double  ppos2d[4][2];
    double  ppos3d[4][2];
    int     dir;
    double  err;
    int     i;

    if( arGetInitRot( marker_info, param->mat, rot ) < 0 ) return -1;

    dir = marker_info->dir;
    ppos2d[0][0] = marker_info->vertex[(4-dir)%4][0];
    ppos2d[0][1] = marker_info->vertex[(4-dir)%4][1];
    ppos2d[1][0] = marker_info->vertex[(5-dir)%4][0];
    ppos2d[1][1] = marker_info->vertex[(5-dir)%4][1];
    ppos2d[2][0] = marker_info->vertex[(6-dir)%4][0];
    ppos2d[2][1] = marker_info->vertex[(6-dir)%4][1];
    ppos2d[3][0] = marker_info->vertex[(7-dir)%4][0];
    ppos2d[3][1] = marker_info->vertex[(7-dir)%4][1];
    ppos3d[0][0] = center[0] - width/2.0;
    ppos3d[0][1] = center[1] + width/2.0;
    ppos3d[1][0] = center[0] + width/2.0;
    ppos3d[1][1] = center[1] + width/2.0;
    ppos3d[2][0] = center[0] + width/2.0;
    ppos3d[2][1] = center[1] - width/2.0;
    ppos3d[3][0] = center[0] - width/2.0;
    ppos3d[3][1] = center[1] - width/2.0;

    for( i = 0; i < AR_GET_TRANS_MAT_MAX_LOOP_COUNT; i++ ) {
        err = arGetTransMat3( rot, ppos2d, ppos3d, 4, conv,
                                   param->dist_factor, param->mat );
        if( err < AR_GET_TRANS_MAT_MAX_FIT_ERROR ) break;
    }
    return err;
}

double arGetTransMat3( double rot[3][3], double ppos2d[][2],
                       double ppos3d[][2], int num, double conv[3][4],
                       double *dist_factor, double cpara[3][4] )
{
    double  off[3], pmax[3], pmin[3];
    double  ret;
    int     i;

    pmax[0]=pmax[1]=pmax[2] = -10000000000.0;
    pmin[0]=pmin[1]=pmin[2] =  10000000000.0;
    for( i = 0; i < num; i++ ) {
        if( ppos3d[i][0] > pmax[0] ) pmax[0] = ppos3d[i][0];
        if( ppos3d[i][0] < pmin[0] ) pmin[0] = ppos3d[i][0];
        if( ppos3d[i][1] > pmax[1] ) pmax[1] = ppos3d[i][1];
        if( ppos3d[i][1] < pmin[1] ) pmin[1] = ppos3d[i][1];
/*
        if( ppos3d[i][2] > pmax[2] ) pmax[2] = ppos3d[i][2];
        if( ppos3d[i][2] < pmin[2] ) pmin[2] = ppos3d[i][2];
*/
    }
    off[0] = -(pmax[0] + pmin[0]) / 2.0;
    off[1] = -(pmax[1] + pmin[1]) / 2.0;
    off[2] = -(pmax[2] + pmin[2]) / 2.0;
    for( i = 0; i < num; i++ ) {
        pos3d[i][0] = ppos3d[i][0] + off[0];
        pos3d[i][1] = ppos3d[i][1] + off[1];
/*
        pos3d[i][2] = ppos3d[i][2] + off[2];
*/
        pos3d[i][2] = 0.0;
    }

    ret = arGetTransMatSub( rot, ppos2d, pos3d, num, conv,
                            dist_factor, cpara );

    conv[0][3] = conv[0][0]*off[0] + conv[0][1]*off[1] + conv[0][2]*off[2] + conv[0][3];
    conv[1][3] = conv[1][0]*off[0] + conv[1][1]*off[1] + conv[1][2]*off[2] + conv[1][3];
    conv[2][3] = conv[2][0]*off[0] + conv[2][1]*off[1] + conv[2][2]*off[2] + conv[2][3];

    return ret;
}

int arGetAngle( double rot[3][3], double *wa, double *wb, double *wc )
{
    double      a, b, c;
    double      sina, cosa, sinb, cosb, sinc, cosc;
#if CHECK_CALC
double   w[3];
int      i;
for(i=0;i<3;i++) w[i] = rot[i][0];
for(i=0;i<3;i++) rot[i][0] = rot[i][1];
for(i=0;i<3;i++) rot[i][1] = rot[i][2];
for(i=0;i<3;i++) rot[i][2] = w[i];
#endif

    if( rot[2][2] > 1.0 ) {
        /* printf("cos(beta) = %f\n", rot[2][2]); */
        rot[2][2] = 1.0;
    }
    else if( rot[2][2] < -1.0 ) {
        /* printf("cos(beta) = %f\n", rot[2][2]); */
        rot[2][2] = -1.0;
    }
    cosb = rot[2][2];
    b = acos( cosb );
    sinb = sin( b );
    if( b >= 0.000001 || b <= -0.000001) {
        cosa = rot[0][2] / sinb;
        sina = rot[1][2] / sinb;
        if( cosa > 1.0 ) {
            /* printf("cos(alph) = %f\n", cosa); */
            cosa = 1.0;
            sina = 0.0;
        }
        if( cosa < -1.0 ) {
            /* printf("cos(alph) = %f\n", cosa); */
            cosa = -1.0;
            sina =  0.0;
        }
        if( sina > 1.0 ) {
            /* printf("sin(alph) = %f\n", sina); */
            sina = 1.0;
            cosa = 0.0;
        }
        if( sina < -1.0 ) {
            /* printf("sin(alph) = %f\n", sina); */
            sina = -1.0;
            cosa =  0.0;
        }
        a = acos( cosa );
        if( sina < 0 ) a = -a;

        sinc =  (rot[2][1]*rot[0][2]-rot[2][0]*rot[1][2])
              / (rot[0][2]*rot[0][2]+rot[1][2]*rot[1][2]);
        cosc =  -(rot[0][2]*rot[2][0]+rot[1][2]*rot[2][1])
               / (rot[0][2]*rot[0][2]+rot[1][2]*rot[1][2]);
        if( cosc > 1.0 ) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = 1.0;
            sinc = 0.0;
        }
        if( cosc < -1.0 ) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = -1.0;
            sinc =  0.0;
        }
        if( sinc > 1.0 ) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = 1.0;
            cosc = 0.0;
        }
        if( sinc < -1.0 ) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = -1.0;
            cosc =  0.0;
        }
        c = acos( cosc );
        if( sinc < 0 ) c = -c;
    }
    else {
        a = b = 0.0;
        cosa = cosb = 1.0;
        sina = sinb = 0.0;
        cosc = rot[0][0];
        sinc = rot[1][0];
        if( cosc > 1.0 ) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = 1.0;
            sinc = 0.0;
        }
        if( cosc < -1.0 ) {
            /* printf("cos(r) = %f\n", cosc); */
            cosc = -1.0;
            sinc =  0.0;
        }
        if( sinc > 1.0 ) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = 1.0;
            cosc = 0.0;
        }
        if( sinc < -1.0 ) {
            /* printf("sin(r) = %f\n", sinc); */
            sinc = -1.0;
            cosc =  0.0;
        }
        c = acos( cosc );
        if( sinc < 0 ) c = -c;
    }

    *wa = a;
    *wb = b;
    *wc = c;

    return 0;
}

int arGetNewMatrix( double a, double b, double c,
                    double trans[3], double trans2[3][4],
                    double cpara[3][4], double ret[3][4] )
{
    double   cpara2[3][4];
    double   rot[3][3];
    int      i, j;

    arGetRot( a, b, c, rot );

    if( trans2 != NULL ) {
        for( j = 0; j < 3; j++ ) {
            for( i = 0; i < 4; i++ ) {
                cpara2[j][i] = cpara[j][0] * trans2[0][i]
                             + cpara[j][1] * trans2[1][i]
                             + cpara[j][2] * trans2[2][i];
            }
        }
    }
    else {
        for( j = 0; j < 3; j++ ) {
            for( i = 0; i < 4; i++ ) {
                cpara2[j][i] = cpara[j][i];
            }
        }
    }

    for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 3; i++ ) {
            ret[j][i] = cpara2[j][0] * rot[0][i]
                      + cpara2[j][1] * rot[1][i]
                      + cpara2[j][2] * rot[2][i];
        }
        ret[j][3] = cpara2[j][0] * trans[0]
                  + cpara2[j][1] * trans[1]
                  + cpara2[j][2] * trans[2]
                  + cpara2[j][3];
    }

    return(0);
} 
 

double arModifyMatrix( double rot[3][3], double trans[3], double cpara[3][4],
                             double vertex[][3], double pos2d[][2], int num )
{
    double    factor;
    double    a, b, c;
    double    a1, b1, c1;
    double    a2, b2, c2;
    double    ma = 0.0, mb = 0.0, mc = 0.0;
    double    combo[3][4];
    double    hx, hy, h, x, y;
    double    err, minerr;
    int       t1, t2, t3;
    int       s1 = 0, s2 = 0, s3 = 0;
    int       i, j;

    arGetAngle( rot, &a, &b, &c );

    a2 = a;
    b2 = b;
    c2 = c;
    factor = 10.0*MD_PI/180.0;
    for( j = 0; j < 10; j++ ) {
        minerr = 1000000000.0;
        for(t1=-1;t1<=1;t1++) {
        for(t2=-1;t2<=1;t2++) {
        for(t3=-1;t3<=1;t3++) {
            a1 = a2 + factor*t1;
            b1 = b2 + factor*t2;
            c1 = c2 + factor*t3;

            arGetNewMatrix( a1, b1, c1, trans, NULL, cpara, combo );

            err = 0.0;
            for( i = 0; i < num; i++ ) {
                hx = combo[0][0] * vertex[i][0]
                   + combo[0][1] * vertex[i][1]
                   + combo[0][2] * vertex[i][2]
                   + combo[0][3];
                hy = combo[1][0] * vertex[i][0]
                   + combo[1][1] * vertex[i][1]
                   + combo[1][2] * vertex[i][2]
                   + combo[1][3];
                h  = combo[2][0] * vertex[i][0]
                   + combo[2][1] * vertex[i][1]
                   + combo[2][2] * vertex[i][2]
                   + combo[2][3];
                x = hx / h;
                y = hy / h;

                err += (pos2d[i][0] - x) * (pos2d[i][0] - x)
                     + (pos2d[i][1] - y) * (pos2d[i][1] - y);
            }

            if( err < minerr ) {
                minerr = err;
                ma = a1;
                mb = b1;
                mc = c1;
                s1 = t1; s2 = t2; s3 = t3;
            }
        }
        }
        }

        if( s1 == 0 && s2 == 0 && s3 == 0 ) factor *= 0.5;
        a2 = ma;
        b2 = mb;
        c2 = mc;
    }

    arGetRot( ma, mb, mc, rot );

/*  printf("factor = %10.5f\n", factor*180.0/MD_PI); */

    return minerr/num;
}
 


static double arGetTransMatSub( double rot[3][3], double ppos2d[][2],
                                double pos3d[][3], int num, double conv[3][4],
                                double *dist_factor, double cpara[3][4] )
{
    ARMat   *mat_a, *mat_b, *mat_c, *mat_d, *mat_e, *mat_f;
    double  trans[3];
    double  wx, wy, wz;
    double  ret;
    int     i, j;

    mat_a = arMatrixAlloc( num*2, 3 );
    mat_b = arMatrixAlloc( 3, num*2 );
    mat_c = arMatrixAlloc( num*2, 1 );
    mat_d = arMatrixAlloc( 3, 3 );
    mat_e = arMatrixAlloc( 3, 1 );
    mat_f = arMatrixAlloc( 3, 1 );

    if( arFittingMode == AR_FITTING_TO_INPUT ) {
        for( i = 0; i < num; i++ ) {
            arParamIdeal2Observ(dist_factor, ppos2d[i][0], ppos2d[i][1],
                                             &pos2d[i][0], &pos2d[i][1]);
        }
    }
    else {
        for( i = 0; i < num; i++ ) {
            pos2d[i][0] = ppos2d[i][0];
            pos2d[i][1] = ppos2d[i][1];
        }
    }

    for( j = 0; j < num; j++ ) {
        wx = rot[0][0] * pos3d[j][0]
           + rot[0][1] * pos3d[j][1]
           + rot[0][2] * pos3d[j][2];
        wy = rot[1][0] * pos3d[j][0]
           + rot[1][1] * pos3d[j][1]
           + rot[1][2] * pos3d[j][2];
        wz = rot[2][0] * pos3d[j][0]
           + rot[2][1] * pos3d[j][1]
           + rot[2][2] * pos3d[j][2];
        mat_a->m[j*6+0] = mat_b->m[num*0+j*2] = cpara[0][0];
        mat_a->m[j*6+1] = mat_b->m[num*2+j*2] = cpara[0][1];
        mat_a->m[j*6+2] = mat_b->m[num*4+j*2] = cpara[0][2] - pos2d[j][0];
        mat_c->m[j*2+0] = wz * pos2d[j][0]
               - cpara[0][0]*wx - cpara[0][1]*wy - cpara[0][2]*wz;
        mat_a->m[j*6+3] = mat_b->m[num*0+j*2+1] = 0.0;
        mat_a->m[j*6+4] = mat_b->m[num*2+j*2+1] = cpara[1][1];
        mat_a->m[j*6+5] = mat_b->m[num*4+j*2+1] = cpara[1][2] - pos2d[j][1];
        mat_c->m[j*2+1] = wz * pos2d[j][1]
               - cpara[1][1]*wy - cpara[1][2]*wz;
    }
    arMatrixMul( mat_d, mat_b, mat_a );
    arMatrixMul( mat_e, mat_b, mat_c );
    arMatrixSelfInv( mat_d );
    arMatrixMul( mat_f, mat_d, mat_e );
    trans[0] = mat_f->m[0];
    trans[1] = mat_f->m[1];
    trans[2] = mat_f->m[2];

    ret = arModifyMatrix( rot, trans, cpara, pos3d, pos2d, num );

    for( j = 0; j < num; j++ ) {
        wx = rot[0][0] * pos3d[j][0]
           + rot[0][1] * pos3d[j][1]
           + rot[0][2] * pos3d[j][2];
        wy = rot[1][0] * pos3d[j][0]
           + rot[1][1] * pos3d[j][1]
           + rot[1][2] * pos3d[j][2];
        wz = rot[2][0] * pos3d[j][0]
           + rot[2][1] * pos3d[j][1]
           + rot[2][2] * pos3d[j][2];
        mat_a->m[j*6+0] = mat_b->m[num*0+j*2] = cpara[0][0];
        mat_a->m[j*6+1] = mat_b->m[num*2+j*2] = cpara[0][1];
        mat_a->m[j*6+2] = mat_b->m[num*4+j*2] = cpara[0][2] - pos2d[j][0];
        mat_c->m[j*2+0] = wz * pos2d[j][0]
               - cpara[0][0]*wx - cpara[0][1]*wy - cpara[0][2]*wz;
        mat_a->m[j*6+3] = mat_b->m[num*0+j*2+1] = 0.0;
        mat_a->m[j*6+4] = mat_b->m[num*2+j*2+1] = cpara[1][1];
        mat_a->m[j*6+5] = mat_b->m[num*4+j*2+1] = cpara[1][2] - pos2d[j][1];
        mat_c->m[j*2+1] = wz * pos2d[j][1]
               - cpara[1][1]*wy - cpara[1][2]*wz;
    }
    arMatrixMul( mat_d, mat_b, mat_a );
    arMatrixMul( mat_e, mat_b, mat_c );
    arMatrixSelfInv( mat_d );
    arMatrixMul( mat_f, mat_d, mat_e );
    trans[0] = mat_f->m[0];
    trans[1] = mat_f->m[1];
    trans[2] = mat_f->m[2];

    ret = arModifyMatrix( rot, trans, cpara, pos3d, pos2d, num );

    arMatrixFree( mat_a );
    arMatrixFree( mat_b );
    arMatrixFree( mat_c );
    arMatrixFree( mat_d );
    arMatrixFree( mat_e );
    arMatrixFree( mat_f );

    for( j = 0; j < 3; j++ ) {
        for( i = 0; i < 3; i++ ) conv[j][i] = rot[j][i];
        conv[j][3] = trans[j];
    }

    return ret;
} 