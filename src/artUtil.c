#ifndef ARTUTIL_H
#define ARTUTIL_H

#include "artoolkit/sysconfig.h"


static int      ss, sms;

void artSleep(int msec)
{
	#ifndef _WIN32
    struct timespec  req;

    req.tv_sec = 0;
    req.tv_nsec = msec* 1000;
    nanosleep( &req, NULL );
	#else
	Sleep(msec);
	#endif
}

double artTimer()
{
#ifdef _WIN32
    struct _timeb sys_time;
    double             tt;
    int                s1, s2;

    _ftime(&sys_time);
    s1 = sys_time.time  - ss;
    s2 = sys_time.millitm - sms;
#else
    struct timeval     time;
    double             tt;
    int                s1, s2;

#if defined(__linux) || defined(__APPLE__)
    gettimeofday( &time, NULL );
#else
    gettimeofday( &time );
#endif
    s1 = time.tv_sec  - ss;
    s2 = time.tv_usec/1000 - sms;
#endif

    tt = (double)s1 + (double)s2 / 1000.0;

    return( tt );
}

void artTimerReset(void)
{
#ifdef _WIN32
    struct _timeb sys_time;

    _ftime(&sys_time);
    ss  = sys_time.time;
    sms = sys_time.millitm;
#else
    struct timeval     time;

#if defined(__linux) || defined(__APPLE__)
    gettimeofday( &time, NULL );
#else
    gettimeofday( &time );
#endif
    ss  = time.tv_sec;
    sms = time.tv_usec / 1000;
#endif
}

#endif