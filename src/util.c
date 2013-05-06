/*
 * This file is part of liblnxproc.
 *
 *  liblnxproc is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  liblnxproc is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with liblnxproc.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2013 Paul Hewlett, phewlett76@gmail.com
 *
 */

#include <sys/time.h>
#include <stddef.h>
#include <stdio.h>

long
lnxproc_timediff(struct timeval *tv1, struct timeval *tv2)
{
    long res = 0;
    if( tv1 && tv2 ) {
        long secdiff = tv1->tv_sec - tv2->tv_sec;
        long usecdiff = (long)tv1->tv_usec - (long)tv2->tv_usec;
        if( usecdiff < 0 ) {
            usecdiff += 1000000;
            secdiff -= 1;
        }
        res = usecdiff + (1000000 * secdiff);
    }
    return res;
}

char *
lnxproc_timeval_print(struct timeval *tv, char *buf, size_t len )
{
    if( tv && buf && (len > 0 )) {
        snprintf(buf,len,"%lu.%06lu", (unsigned long) tv->tv_sec,
                                      (unsigned long) tv->tv_usec);
    }
    return buf;
}
/*
 * vim: tabstop=4:softtabstop=4:shiftwidth=4:expandtab
 */
